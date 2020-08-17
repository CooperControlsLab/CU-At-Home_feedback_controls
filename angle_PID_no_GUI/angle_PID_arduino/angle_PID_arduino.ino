#include <Arduino.h>
#include <PID_v1.h>

#define ENC_CHAN_A 18
#define ENC_CHAN_B 19
#define DIR_A 12
#define DIR_B 13
#define PWM_A 3
#define PWM_B 11
#define BRK_A 9
#define BRK_B 8
#define PPR 600 //pulses per revolution of the encoder

//Global Variables
//Encoder Counting Variables, Encoder Parameters
volatile long enc_count=0; //Encoder counting variable
float deg = 0;    // Angle rotated
float start_time = 0;
////PID controller variables
//double input, output;
//double setpoint=25;
//double kp = 1.5;
//double ki = 0.1;
//double kd = 0.5;
//int base_pwm = 50; //Default power for both propellers
//PID balance(&input, &output, &setpoint, kp, ki, kd, DIRECT);


int main(void){
  init();
  #if defined(USBCON)
      USBDevice.attach();
  #endif
  Serial.begin(19200);
  
  //Setup  
  //Set digial pin modes
  pinMode(ENC_CHAN_A, INPUT_PULLUP);
  pinMode(ENC_CHAN_B, INPUT_PULLUP);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  digitalWrite(BRK_B,HIGH);  // Prevent Motor Rotate
  //Set interrupt connection
  attachInterrupt(digitalPinToInterrupt(ENC_CHAN_A), do_count, RISING);

  //Handshake
  while(!Serial.available()){Serial.println('A');delay(300);}
  if(Serial.read() == 'A'){Serial.println("Contact established");delay(1000);}
  
  
  //Motor Step Response
  start_time = millis();
  for(int i = 0;i<3;i++){
      digitalWrite(DIR_B, LOW); //Establishes forward direction of Channel B
      digitalWrite(BRK_B, LOW);   //Disengage the Brake for Channel B
      analogWrite(PWM_B, 150);   //Spins the motor on Channel B at full speed
      delay_enc(1000);
      digitalWrite(BRK_B, HIGH);   //Lock Motor at Channel B
      analogWrite(PWM_B, 0);   //Disengage motor on Channel B
      delay_enc(1000);
//      enc_count = 0;
  }
  while(1){if(Serial.available()){Serial.println("End");delay(300);}} // Send "End" to terminate python code
  return 0;
}

void do_count(){
  if(digitalRead(ENC_CHAN_B) == 1){enc_count++;} //If chan B is HIGH, increment
  else {enc_count--;} //Else decrement
}

void delay_enc(float t){
  float t_max = millis()+t;
  float cur_time = millis();
  while(cur_time < t_max){
    if(Serial.read() == 'B'){
//    if(Serial.available()){
      Serial.print(cur_time-start_time);
      Serial.print(",");
      Serial.println(enc_count);
    }
    cur_time = millis();
  }
}
