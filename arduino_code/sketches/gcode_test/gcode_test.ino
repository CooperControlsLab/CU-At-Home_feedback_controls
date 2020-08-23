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
char incoming_char;
char cmd [200];
int cmd_index = 0;
double input,motorPWM,setpoint,kp,ki,kd; // Controller params
int lowerOutputLimit = -255;  // Controller upper limit
int upperOutputLimit = 255;   // Controller lower limit

double enc_count,enc_deg;
double motor_speed;
double valA,valB;

SerialComms com;

// Initialize PID motor controller
PID motor_controller(&input, &motorPWM, &com.setpoint, kp, ki, kd, DIRECT);  // Set up PID controller

void setup() {
  Serial.begin(9600);  // Begins Serial communication
  
  motor_controller.SetMode(AUTOMATIC); // Setup controller mode
  motor_controller.SetOutputLimits(-255,255); // Default controller to use full range of PWM
  
  //Encoder Setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), pulseB, CHANGE);
  
  //Motor Setup
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(PWM_B,LOW);  // Breaks Motor
}

void loop() {
  if(com.mode != 0){
    handle_command();  //Process incoming command
    update_control_params();    //Check and update gains
    motor_controller.Compute();

    analogWrite(PWM_B,motorPWM);
  }
}

double calc_motor_speed(){
  motor_speed = 10;
}

void update_control_params(){
  // Update input
  if(com.labType == 0){  // Angle Control
    input = enc_deg;
  }
  else if(com.labType == 1){  // Speed Control
    calc_motor_speed();
    input = motor_speed;
  }
  if(kp != com.kp || ki!= com.ki || kd!= com.kd){
    kp = com.kp; ki = com.ki; kd = com.kd;
    motor_controller.SetTunings(kp,ki,kd);  // Update gains
  }
  if(lowerOutputLimit != com.lowerOutputLimit || upperOutputLimit != com.upperOutputLimit){
    lowerOutputLimit = com.lowerOutputLimit;
    upperOutputLimit = com.upperOutputLimit;
    motor_controller.SetOutputLimits(lowerOutputLimit,upperOutputLimit);
  }
}

// Arduino command handler
void handle_command(){
  if(Serial.available() != 0){
    incoming_char = Serial.read();
    cmd[cmd_index] = incoming_char;
    if(incoming_char == '\0'){
      Serial.println("End of line, processing commands!");
      com.process_command(cmd);
      // Check and if there is a request, send data
      if(com.write_data == 1){
        Serial.print("T");Serial.print(micros());Serial.print(',');
        Serial.print('S');Serial.print(setpoint);Serial.print(',');
        
        if(com.labType == 0){ // Angle
          Serial.print('A');
          Serial.print(enc_deg);
        }
        else if(com.labType == 1){
          Serial.print('V');
          Serial.print(motor_speed);
        }
        Serial.print(',');
        Serial.print('Q');Serial.print(motorPWM);Serial.print(',');Serial.println('\0');
        com.write_data = 0; // Reset write data flag
      }
      // Reset command buffer
      cmd_index = 0;
      char cmd [200];
    }
    else{cmd_index ++;}
  }
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
