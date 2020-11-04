#include "SerialComms.h"
#include <drone_control_hardware_config.h>
#include <PID_beard.h>
#include <Differentiator.h>

#define DEG_TO_RAD 2*3.14159/360
#define RPM_TO_RADS 0.104719755

const int storage_length = 200;
//AnalysisData dataArray[storage_length];
int time[storage_length];
//int index[storage_length];
int velocity[storage_length];
//int position[storage_length];
//int inputData[storage_length];

//Timing Parameters for fixed interval calculations for PID and derivitave
unsigned long prev_micros = 0;
unsigned long current_micros;

//Global Variables
double setpoint = 100;
double lowerOutputLimit, upperOutputLimit; // Controller params
double base_motor_voltage = 1; //Base voltage that both props will have normally

volatile double enc_count;  //Encoder "ticks" counted, Enc ++ = CW, Enc -- = CCW
double enc_deg; // Encoder position in degrees
double angular_velocity; //Angular velocity of the motor
double prev_deg =0; //Previous encoder position for angular velocity calculation


SerialComms com;  //Serial Communications class instantiation

//instantiate beard PID controller
double kp = 5;
double ki = 0;
double kd = 1;
double lowlimit = -2;
double uplimit = 2;
double sigma = 0.01;
double sample_period = 0.005; //in sec
bool flag = true;
double pid_output=0;
bool controller_on = true;

Differentiator diff(sigma, sample_period);
PIDControl controller(kp, ki, kd, lowlimit, uplimit, sigma, sample_period, flag);


//***********************************************************************************
void setup() {
  Serial.begin(500000);  // Begins Serial communication

  //Encoder Setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), pulseB, CHANGE);

  //Motor Setup
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0); // Breaks Motor
  
  //Initialize variables
  current_micros = micros();
  prev_micros = current_micros;
  prev_deg = 0;

  //Force com values for debug
  com.mode = 0;
  com.labType = 0;
}

//***********************************************************************************
void loop() {
   com.handle_command();  //Process incoming command
   update_control_params();    //Update gains, update inputs based on labtype
   compute_motor_voltage(); // Update controller input, compute motor voltage and write to motor
   com.send_data(enc_deg, angular_velocity, pid_output);
//  Serial.println(enc_deg);
}

//*****************************************************//
void update_control_params() {
  //Check for calibration procedure
  if(com.calibration_start)
  {
    //Turn 1 motor on for x seconds until we feel confident the system is holding at the end stop
    //Set enc_count to the count it should be if we want 0 to be perfectly level

    //Set
    analogWrite(PWM_A, volts_to_PWM(0));
    analogWrite(PWM_B, volts_to_PWM(SUPPLY_VOLTAGE/2));

    //Hold motor for 5 seconds
    delay(5000);

    //Set enc_count to 94deg/2 
    enc_count = (PPR/360.0) * (-94/2);

    //Shut off motors
    analogWrite(PWM_A, volts_to_PWM(0));
    analogWrite(PWM_B, volts_to_PWM(0));

    //Reset calibration flag to false
    com.calibration_start = false;
  }
  
  //Update Setpoint
  if(setpoint != com.setpoint){
    setpoint = com.setpoint;
  }

  //Update Mode
  if (com.mode == 0) {
    controller_on = false;  // M0 - Controller Off, set motor output to zero
    pid_output = 0; //Force PID output to zero
    base_motor_voltage = 0; //Force base motor voltages to zero
    analogWrite(PWM_A, 0); //Write pwm to motors to zero to shut down motors
    analogWrite(PWM_B, 0);
  }
  else if (com.mode == 1) {
    // M1 - Controller On
    controller_on = true; //Turn controller on
    base_motor_voltage = 1; //Reset base motor voltage so motors will spin
  }

  //Update gains
  if (kp != com.kp || ki != com.ki || kd != com.kd) {
    kp = com.kp; ki = com.ki; kd = com.kd;  // Checking for difference before setting to prevent jitter
    // Update gains
    controller.update_gains(kp, ki, kd);
//    Serial.print(" kp test: "); Serial.println(controller.kp, 10);
  }

  if (lowerOutputLimit != com.lowerOutputLimit || upperOutputLimit != com.upperOutputLimit) {
    lowerOutputLimit = com.lowerOutputLimit;
    upperOutputLimit = com.upperOutputLimit;
    controller.upperLimit = upperOutputLimit;
    controller.lowerLimit = lowerOutputLimit;
    
  }

  //Update Sample Time
  if (sample_period != com.sampleTime) {
    sample_period = com.sampleTime;
    controller.update_time_parameters(sample_period, sigma); //Update controller sample period
    diff.update_time_parameters(sample_period, sigma);
  }
}

//*****************************************************//
// Computes and executes motor voltage based on appropriate labtypes
void compute_motor_voltage() {
  enc_deg = count_to_deg(enc_count); // Retrieve Current Position in Radians
  current_micros = micros(); //Get current microseconds
  
  switch (com.labType) {
    case 0: // Angle Control
      if(controller_on){
        //If sample period amount has passed do processing
        if((current_micros - prev_micros) >= (controller.Ts * 1000000.0))
        {
          
          //Calculate PID output
          pid_output = controller.PID(setpoint*DEG_TO_RAD, enc_deg*DEG_TO_RAD);
//          Serial.print(" | dt: "); Serial.print(current_micros - prev_micros);
//          Serial.print(" | setpoint: "); Serial.print(setpoint);
//          Serial.print(" | encdeg: "); Serial.print(enc_deg);
//          Serial.print(" | Ts: "); Serial.print(controller.Ts, 10);
//          Serial.print(" | kp: "); Serial.print(controller.kp);
//          Serial.print(" | beta: "); Serial.print(controller.beta);
//          Serial.print(" | sigma: "); Serial.print(controller.sigma);
//          Serial.print(" | pidout: ");
//          Serial.println(pid_output);
      
          //update prev variables
          prev_micros = current_micros;
          prev_deg = enc_deg;
        }
        
      }
      pid_output = pid_output_signal_conditioning(pid_output);
      update_motor_voltage(pid_output);
      break;

    case 1: // Speed Control
      if(controller_on){
        //If sample period amount has passed do processing
        if((current_micros - prev_micros) >= (controller.Ts * 1000000.0))
        {
          //Calculate angular velocity from derivative
          angular_velocity = diff.differentiate(enc_deg*DEG_TO_RAD);
          
          //Calculate PID output
          pid_output = controller.PID(setpoint*RPM_TO_RADS, angular_velocity);
          
          //update prev variables
          prev_micros = current_micros;
          prev_deg = enc_deg;
        } 
      }
      pid_output = pid_output_signal_conditioning(pid_output);
      update_motor_voltage(pid_output);
      break;

    case 2: // Open Loop Speed Control
//      if (com.mode == 1) {pid_output = com.open_loop_voltage;} // Chnage motor voltage to open loop voltage if controller is on
//      // else if (com.mode == 0) {
//      //   motor_voltage = 0;
//      // }
//      analogWrite(PWM_B, volts_to_PWM(pid_output));
      break;
      
    default: break; // Null default
  }
}

//*****************************************************//
void update_motor_voltage(double voltage){
  //Fix direction based on +/-

  digitalWrite(DIR_A, HIGH); // CCW
  digitalWrite(DIR_B, HIGH); // CW
  
  //set pwm based off of bsae motor voltage + voltage
  analogWrite(PWM_A, volts_to_PWM(abs(base_motor_voltage + voltage)));
  analogWrite(PWM_B, volts_to_PWM(abs(base_motor_voltage - voltage)));
}

//*****************************************************//
// Voltage and PWM duty cycle conversion
int volts_to_PWM(double voltage) {

  // Convert voltage to PWM duty cycle percent relative to the power supply voltage
  return constrain(round((voltage / SUPPLY_VOLTAGE) * 255), -255, 255);
}

double PWM_to_volts(int PWM) {
  return double(PWM / 255) * SUPPLY_VOLTAGE;
}

//*************************************//
// pid output signal conditioning (clipping based off of lower and upper limits)
double pid_output_signal_conditioning(double val){
  val = constrain(val, lowerOutputLimit, upperOutputLimit);
  return val;
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
