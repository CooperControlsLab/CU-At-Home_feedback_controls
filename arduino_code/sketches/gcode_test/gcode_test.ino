#include "g_code_interpreter.h"
//#include <string.h>
#include <PID_v1.h>

// Define hardware pins
#define ENC_A 2   //Encoder A
#define ENC_B 3   //Encoder B
#define PPR 211.2 //Encoder pulses per revolution 

#define DIR_B 13 //Motor Direction
#define PWM_B 11 //Motor PWM
#define BRK_B 8  //Motor Break


//Global Variables
int labType;
int mode;
double setpoint;
SerialComms com;


// Initialize PID motor controller
PID motor_controller(&input, &motorPWM, &setpoint, kp, ki, kd, DIRECT);  // Set up PID controller

void setup() {
  Serial.begin(9600);  // Begins Serial communication
  
  motor_controller.SetMode(AUTOMATIC); // Setup controller mode
  motor_controller.SetOutputLimits(-255,255); // Default controller to use full power
  
  //Encoder Setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), pulseB, CHANGE);
  
  //Motor Setup
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(PWM_B,LOW);  // Breaks Motor

  start_time = micros(); //Loop start time
}

void loop() {
  //Process incoming command
  if(Serial.available() != 0){
    incoming_char = Serial.read();
    cmd[cmd_index] = incoming_char;
    if(incoming_char == '\0'){
      Serial.println("End of line, processing commands!");
      com.process_command(cmd);
      update_params();
      cmd_index = 0;
      char cmd [200];
    }
    else{cmd_index ++;}
  }

  // Determine lab type, compute appropriate setpoint and input signal
  if(com.labType == 0){  // Motor Angle Control
//    input = coef*enc_count;  // update input as angle
  }
  else if(com.labType == 1){ // Motor Speed Control
      input = calc_motor_speed();  // update input as motor speed
  }

  //Update PID limits and Compute PWM
  motor_controller.SetOutputLimits(lowerOutputLimit,upperOutputLimit);
  motor_controller.Compute();  //PID compute, update output
}

void update_params(){
  labType = com.labType;
  setpoint = com.setpoint;
  mode = com.mode;
  lowerOutputLimit = com.lowerOutputLimit;
  upperOutputLimit = com.upperOutputLimit;
  sampleTime = com.sampleTime;
  kp = com.kp;
  ki = com.ki;
  kd = com.kd;
  motor_controller.SetTunings(kp, ki, kd); //Update Controller Gains
}

double calc_motor_speed(){
  for(int i = 0;i<sizeof(enc_count_memory);i++){
    diff += enc_count_memory[i+1] - enc_count_memory[i];
    enc_count_memory[i] = enc_count_memory[i+1];
  }
  diff += enc_count - enc_count_memory[sizeof(enc_count_memory)];
  enc_count_memory[sizeof(enc_count_memory)] = enc_count;
  diff = 0;
  
  return double(diff/sizeof(enc_count_memory));
}

//Encoder interrupts
void pulseA() {
  valA = digitalRead(ENC_A);
  valB = digitalRead(ENC_B);
  
  if(valA == HIGH){  // A Rise
    if(valB == LOW){enc_count ++;}  // CW
    else{enc_count --;}  // CCW
  }
  else{  // A fall
    if(valB == HIGH){enc_count ++;}  // CW
    else{enc_count --;}  //CCW
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
