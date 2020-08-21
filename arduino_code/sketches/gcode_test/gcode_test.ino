#include "g_code_interpreter.h"
//#include <string.h>
//#include <motor_control.h>
#include <PID_v1.h>

//Global Variables

char command[200] = {0};
int command_index =0;
double kp;
double ki;
double kd;
double setpoint;
SerialComms com;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  char a[20] = "S0,I1,P2252.025,D3,\0";
  char b[20] = "S1,Z20,\0";
  com.process_command(a);
  com.process_command(b);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() != 0)
  {
    
    command[command_index] = Serial.read();
    if(command[command_index] == '\0')
    { com.process_command(command);
      //reset command_index
      //reset command char array
      //update global variables with data from processed command
      }
    command_index++;
    
  }
  //Compute
  if(com.labType == 0) { angle.Compute(); }
  if(com.labType == 1) { velocity.Compute(); }

  //update input
  if(com.labType == 0) { //do angle shit}
  if(com.labType == 1) { //do velocity calc etc..
  }
  //update output
}

void update()
{
  kp = com.kp;
  ki = com.ki;
  
  if(com.labType == 0) {angle.SetMode(com.mode); velocity.SetMode(!com.mode);}
  if(com.labType == 1) {angle.SetMode(!com.mode); velocity.SetMode(com.mode);}
}
