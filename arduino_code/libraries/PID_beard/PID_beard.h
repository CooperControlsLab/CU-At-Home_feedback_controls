
class PIDControl{

public:
    //Construtors
    PIDControl(double, double, double, double, double, double, bool);

    //class variables
    double kp; //Proportional gain
    double ki; //Integral gain
    double kd; //Derivative gain
    double limit; //Output saturates at this limit
    double sigma; //dirty derivative bandwidth = 1/sigma
    double Ts; //sample period in seconds
    double beta; //(2.0*sigma-Ts)/(2.0*sigma+Ts)
    bool flag; //derivative on error_dot(true) or y_dot(false)
    double y_dot; //estimated derivative of y
    double y_d1; //signal y delayed by one sample
    double error_dot; //estimated derivative of error
    double error_d1; //error delayed by one sample
    double integrator; //integrator value

    //class methods
    double PID(double,double);
    double PD(double, double);
    double saturate(double);
    void update_time_parameters(double, double);
    void update_gains(double, double, double);
    void setpoint_reset(double, double);
};