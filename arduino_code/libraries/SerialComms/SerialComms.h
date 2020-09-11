#include <Arduino.h>

class SerialComms{
public:
    //Process a command received from serial buffer
    void process_command(char*);
    //Search cmd for letter, return number immideately after letter
    float parse_number(char*, char, int);
    // SerialComms(int*, double*, pwmAngle, pwmVelocity, time);
    SerialComms();

    void handle_command();
    void send_data(double, double, double);

    //-------
    //Serial communication buffer params
    char cmd [200]; //Input command from serial
    int cmd_index; //Current index in cmd[] 
    char incoming_char; //Serial incoming character for "parallel processing" of serial data


    //--------------------------
    //Local variables to hold data from serial stream
    int labType;
    double setpoint;
    int mode;
    int lowerOutputLimit;
    int upperOutputLimit;
    int sampleTime;
    double kp;
    double ki;
    double kd;
    int write_data;
    int open_loop_voltage;
    double feed_forward_voltage;
    double FF_A;
    double FF_B;
    double FF_C;
    bool open_loop_analysis_start;
    double open_loop_analysis_time;
    bool calibration_start;
};


/*
Serial command protocol
H0 - Handshake
R0 - Request data dependent on lab type
S0,P#,I#,D# - Set PID gains on arduino
S1,Z# - Set the setpoint of the controller
S2,Y# - Lab type 0-angle, 1-ang_velocity, 2-openloop
S3,M# - Turn controller on/off
S4,T# - Set sample time
S5,L#,U# - Set lower and upper controller output limits
S6,O# - Open loop PWM
S7,F# - Feedforward gain in volts
S8, T#(seconds to run)  - Start Open Loop Charactization Analysis

Data to python protocol
T# - time in micros
S# - setpoint
A# - value, angle or ang_speed dependent on labtype
Q# - PMW
D#(index),T#(us),P#,V#,I#$D1, etc... \0
*/