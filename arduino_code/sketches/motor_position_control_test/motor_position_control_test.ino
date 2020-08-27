#include "g_code_interpreter.h"
//#include <string.h>
#include <PID_v1.h>

// Define hardware pins
#define ENC_A 2   //Encoder pulse A
#define ENC_B 3   //Encoder pulse B
#define PPR 464.64 //Encoder pulses per revolution 
#define PPR_A 232.8 //Pulse A per r
#define DIR_B 13 //Motor Direction HIGH = CW, LOW = CCW
#define PWM_B 11 //Motor PWM
#define BRK_B 8  //Motor Break Doesn't seem to work, avoid using
#define SUPPLY_VOLTAGE 12 //12V power supply
#define MOVING_AVERAGE_SIZE 50 // Size of moving average array

//Global Variables
char incoming_char;
char cmd [200];
int cmd_index = 0;
double input,motor_voltage,lowerOutputLimit,upperOutputLimit,setpoint,kp,ki,kd; // Controller params
int sampleTime, motor_direction;

volatile double enc_count, enc_deg; // Enc ++ = CW, Enc -- = CCW
double motor_speed;
double motor_speed_array [MOVING_AVERAGE_SIZE];

volatile int valA,valB;
volatile unsigned long prev_pulse_time, pulse_interval;
int stationary_thresh = 80000; //80000 us
double prev_motor_voltage;

SerialComms com;

// Initialize PID motor controller
PID motor_controller(&input, &motor_voltage, &setpoint, kp, ki, kd, DIRECT);  // Set up PID controller

void setup() {
  Serial.begin(115200);  // Begins Serial communication
  
//  com.mode = 1; // Default controller mode to automatic
//  motor_controller.SetOutputLimits(-255,255); // Default controller to use full range of PWM
  
  //Encoder Setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), pulseB, CHANGE);
  
  //Motor Setup
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(PWM_B,LOW);  // Breaks Motor
  
  for(int i=0;i<MOVING_AVERAGE_SIZE;i++){motor_speed_array[i] = 0;} //Initializing motor speed array
}

void loop() {
  handle_command();  //Process incoming command
  enc_deg = double(enc_count/PPR*360);
  update_control_params();    //Update gains, update inputs based on labtype
  compute_motor_voltage(com.labType); // Compute motor voltage and write to motor
  send_data();
}

//*****************************************************//
// Arduino command handler
void handle_command(){
  if(Serial.available() != 0){
    incoming_char = Serial.read();
    cmd[cmd_index] = incoming_char;
    if(incoming_char == '\0' ||incoming_char == '%'){
//      Serial.println("End of line, processing commands!");
      com.process_command(cmd);
      // Reset command buffer
      cmd_index = 0;
      memset(cmd,'\0',sizeof(cmd));
      update_control_params();    //Update gains, update inputs based on labtype
    }
    else{cmd_index ++;}
  }
}
//*****************************************************//
void update_control_params(){
  // Update input    
  if(com.labType == 0){  // Angle Control
    input = enc_deg;
  }
  else if(com.labType == 1){  // Speed Control
    calc_motor_speed();
    input = motor_speed;
  }
  
  //Update Setpoint
  setpoint = com.setpoint;
  
  //Update Mode
  if(com.mode == 0){motor_controller.SetMode(MANUAL); motor_voltage = 0;}  // M0 - Controller Off, set motor output to zero
  else if(com.mode == 1){motor_controller.SetMode(AUTOMATIC);}  // M1 - Controller On
  
  if(kp != com.kp || ki!= com.ki || kd!= com.kd){
    kp = com.kp; ki = com.ki; kd = com.kd;  // Checking for difference before setting to prevent jitter
    motor_controller.SetTunings(kp,ki,kd);  // Update gains
  }

  if(lowerOutputLimit != com.lowerOutputLimit || upperOutputLimit != com.upperOutputLimit){
    lowerOutputLimit = com.lowerOutputLimit;
    upperOutputLimit = com.upperOutputLimit;
    motor_controller.SetOutputLimits(lowerOutputLimit,upperOutputLimit);
  }
  if(sampleTime != com.sampleTime){
    sampleTime = com.sampleTime;
    motor_controller.SetSampleTime(sampleTime);
  }
}

//*****************************************************//
// Computes motor voltage based on appropriate labtypes
// Returns motor_voltage
void compute_motor_voltage(int labtype){
  switch (labtype){
  case 0: // Angle Control
    motor_controller.Compute();
    if(motor_voltage < 0){digitalWrite(DIR_B,LOW);} // CCW
    else{digitalWrite(DIR_B,HIGH);} // CW
    analogWrite(PWM_B,volts_to_PWM(abs(motor_voltage)));
    break;

  case 1: // Speed Control
    prev_motor_voltage = motor_voltage;
    motor_controller.Compute();
    motor_voltage += prev_motor_voltage;
    analogWrite(PWM_B,volts_to_PWM(motor_voltage));
    break;

  case 2: // Open Loop Speed Control
    motor_speed = calc_motor_speed();
    if(com.mode == 1){motor_voltage = volts_to_PWM(com.open_loop_voltage);}
    else if(com.mode == 0){motor_voltage = 0;}
    analogWrite(PWM_B,volts_to_PWM(motor_voltage));
    break;

  default: break; // Null default
  }
}

//*****************************************************//
double calc_motor_speed(){
//  Check if motor is stationary
  if(micros() -  prev_pulse_time > stationary_thresh){
    motor_speed = 0;
  }
  else{
    motor_speed = update_motor_speed_array(motor_direction*double(360.0/PPR_A)*float(1000000.0/pulse_interval));
  }
}

//*****************************************************//
// Update moving average speed array
double update_motor_speed_array(double newVal){
  double sum = 0;
  for(int i = 0;i<MOVING_AVERAGE_SIZE-1;i++){
    motor_speed_array[i] = motor_speed_array[i+1];
    sum += motor_speed_array[i];
  }
  motor_speed_array[MOVING_AVERAGE_SIZE-1] = newVal;
  sum += newVal;
  return double(sum / MOVING_AVERAGE_SIZE);
}

//*****************************************************//
// Send data on request
void send_data(){
  // Check and if there is a request, send data
  if(com.write_data == 1){
    Serial.print("T");Serial.print(micros());Serial.print(',');
    Serial.print('S');Serial.print(setpoint);Serial.print(',');
    Serial.print('A');
    if(com.labType == 0){ // Angle
      Serial.print(enc_deg);
    }
    else if(com.labType == 1 || com.labType == 2){
      Serial.print(motor_speed);
    }
    
    Serial.print(',');
    Serial.print('Q');Serial.print(motor_voltage);Serial.print(',');
    // Serial.print('Q');Serial.print(volts_to_PWM(motor_voltage));Serial.print(',');
    Serial.println('\0');
    com.write_data = 0; // Reset write data flag
  }
}

//*****************************************************//
// Voltage and PWM duty cycle conversion
int volts_to_PWM(double voltage){
  return constrain(round((voltage/SUPPLY_VOLTAGE) * 255),-255,255);
}

double PWM_to_volts(int PWM){
  return double(PWM/255) * SUPPLY_VOLTAGE;
}

//*****************************************************//
//Encoder interrupts
void pulseA() {
  pulse_interval = micros() - prev_pulse_time;
  prev_pulse_time = micros();
  
  valA = digitalRead(ENC_A);
  valB = digitalRead(ENC_B);
  
  if(valA == HIGH){  // A Rise
    if(valB == LOW){enc_count++; motor_direction = 1;}  // CW
    else{enc_count--; motor_direction = -1;}  // CCW
  } 
  else{  // A fall
    if(valB == HIGH){enc_count ++;motor_direction = 1;}  // CW
    else{enc_count --;motor_direction = -1;}  //CCW
  }
}

void pulseB() {
  valA = digitalRead(ENC_A);
  valB = digitalRead(ENC_B);
  
  if(valB == HIGH){  // B rise
    if(valA == HIGH){enc_count ++;}  // CW
    else{enc_count --;}  // CCW
  }
  else{  //B fall
    if(valA == LOW){enc_count ++;}  // CW
    else{enc_count --;}  // CCW
  }
}
