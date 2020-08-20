#include <Arduino.h>

class SerialComms{
    public:
    //Process a command received from serial buffer
    void processCommand(char*);
    //Search cmd for letter, return number immideately after letter
    float parseNumber(char*, char, int);
};


/*
Serial command protocol
H0 - Handshake
R - Request data dependent on lab type
S0,P#,I#,D# - Set PID gains on arduino
S1,Z# - Set the setpoint of the controller
S2,Y# - Lab type 0-angle, 1-ang_velocity
S3,M# - Turn controller on/off
S4,T - Set sample time
S5,L#,U# - Set lower and upper controller output limits

*/