#include "SerialComms.h"
#include <PID_v1.h>
#include <motor_control_hardware_config.h>

unsigned long DELTA_T = 2000; //delta T in us between calculating velocity
// unsigned int stationary_thresh = 80000; //80000 us threshold to set angular velocity to 0 if reached

//Global Variables
double input, setpoint, motor_voltage, kp, ki, kd, lowerOutputLimit, upperOutputLimit, sampleTime; // Controller params

volatile double enc_count;  //Encoder "ticks" counted, Enc ++ = CW, Enc -- = CCW
double enc_deg; // Encoder position in degrees
double motor_speed; //Angular velocity of the motor
double prev_pos; //Previous encoder position for angular velocity calculation

volatile unsigned long prev_micros, current_micros;

SerialComms com;  //Serial Communications class instantiation

// Initialize PID motor controller
PID motor_controller(&input, &motor_voltage, &setpoint, kp, ki, kd, DIRECT);  // Set up PID controller

//***********************************************************************************
void setup() {
  Serial.begin(500000);  // Begins Serial communication

  //Encoder Setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), pulseB, CHANGE);

  //Motor Setup
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(PWM_B, LOW); // Stop Motor on Start Up

  //Initialize variables
  current_micros = micros();
  prev_micros = current_micros;
  prev_pos = 0;
}

//***********************************************************************************
void loop() {
   com.handle_command();  //Process incoming command
   update_control_params();    //Update gains, update inputs based on labtype
   compute_motor_voltage(); // Update controller input, compute motor voltage and write to motor
   com.send_data(enc_deg, motor_speed, motor_voltage);
}

//*****************************************************//
void update_control_params() {
  //Update Setpoint
  setpoint = com.setpoint;

  //Update Mode
  if (com.mode == 0) {
    motor_controller.SetMode(MANUAL);  // M0 - Controller Off, set motor output to zero
    motor_voltage = 0;
  }
  else if (com.mode == 1) {
    motor_controller.SetMode(AUTOMATIC); // M1 - Controller On
  }

  //Update gains
  // motor_controller.SetTunings(com.kp, com.ki, com.kd);

  if (kp != com.kp || ki != com.ki || kd != com.kd) {
    kp = com.kp; ki = com.ki; kd = com.kd;  // Checking for difference before setting to prevent jitter
    motor_controller.SetTunings(kp, ki, kd); // Update gains
  }

  //Update output limits
  // motor_controller.SetOutputLimits(com.lowerOutputLimit, com.upperOutputLimit);


  if (lowerOutputLimit != com.lowerOutputLimit || upperOutputLimit != com.upperOutputLimit) {
    lowerOutputLimit = com.lowerOutputLimit;
    upperOutputLimit = com.upperOutputLimit;
    motor_controller.SetOutputLimits(lowerOutputLimit, upperOutputLimit);
  }

  //Update Sample Time
  // motor_controller.SetSampleTime(sampleTime);

  if (sampleTime != com.sampleTime) {
    sampleTime = com.sampleTime;
    motor_controller.SetSampleTime(sampleTime);
  }
}

//*****************************************************//
// Computes and executes motor voltage based on appropriate labtypes
void compute_motor_voltage() {
  switch (com.labType) {
    case 0: // Angle Control
      enc_deg = count_to_deg(enc_count); // Retrieve Current Position in Radians
      input = enc_deg;
      motor_controller.Compute();

      if (motor_voltage < 0) {
        digitalWrite(DIR_B, LOW); // CCW
        } 
        
      else {
        digitalWrite(DIR_B, HIGH); // CW
      }
      analogWrite(PWM_B, volts_to_PWM(abs(motor_voltage)));
      break;

    case 1: // Speed Control
      calc_motor_speed();
      input = motor_speed;
      motor_controller.Compute();
      motor_voltage += com.FF_A*pow(com.setpoint,2) + com.FF_B*com.setpoint + com.FF_C; // voltage = feedforward voltage + PID voltage, FF voltage = Ax^2 + Bx + C
      analogWrite(PWM_B, volts_to_PWM(motor_voltage));
      break;

    case 2: // Open Loop Speed Control
      calc_motor_speed();
      if (com.mode == 1) {motor_voltage = com.open_loop_voltage;} // Chnage motor voltage to open loop voltage if controller is on
      // else if (com.mode == 0) {
      //   motor_voltage = 0;
      // }
      analogWrite(PWM_B, volts_to_PWM(motor_voltage));
      break;
    default: break; // Null default
  }
}

//*****************************************************//
void calc_motor_speed() {
  enc_deg = count_to_deg(enc_count); // Retrieve Current Position in Degrees
  current_micros = micros();
  unsigned long dt = current_micros - prev_micros;
  if ( dt > DELTA_T)
  {
    //Calculate velocity in rpm
    motor_speed = ((enc_deg - prev_pos) /(dt/(1000000.0 * 60.0)))/360; // rpm = deg/min / (360 deg/rev)
    prev_micros = current_micros;
    prev_pos = enc_deg;
  }
}

//*****************************************************//
// Voltage and PWM duty cycle conversion
int volts_to_PWM(double voltage) {
  return constrain(round((voltage / SUPPLY_VOLTAGE) * 255), -255, 255);
}

double PWM_to_volts(int PWM) {
  return double(PWM / 255) * SUPPLY_VOLTAGE;
}

//*****************************************************//

//Encoder Count to Radians
double count_to_deg(double count){
  return (double(count / PPR) * 360);  // rad = (count / pulse/rev) * 360 deg/rev
}

//Encoder interrupts
void pulseA(){
  int valA = digitalRead(ENC_A);
  int valB = digitalRead(ENC_B);

  if (valA == HIGH) { // A Rise
    if (valB == LOW) {
      enc_count++;  // CW
    }
    else {
      enc_count--;  // CCW
    }
  }
  else { // A fall
    if (valB == HIGH) {
      enc_count ++;  // CW
    }
    else {
      enc_count --;  //CCW
    }
  }
}

void pulseB(){
  int valA = digitalRead(ENC_A);
  int valB = digitalRead(ENC_B);

  if (valB == HIGH) { // B rise
    if (valA == HIGH) {
      enc_count ++; // CW
    }
    else {
      enc_count --; // CCW
    }
  }
  else { //B fall
    if (valA == LOW) {
      enc_count ++; // CW
    }
    else {
      enc_count --; // CCW
    }
  }
}
