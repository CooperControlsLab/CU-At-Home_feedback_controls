#include <PID_beard.h>
#include <Arduino.h>

//Constructor logic
PIDControl::PIDControl(double p, double i, double d, double lim, double sigm, double t, bool fl){

    kp = p;
    ki = i;
    kd = d;
    limit = lim;
    sigma = sigm;
    Ts = t;
    flag = fl;
    beta = (2.0*sigma - Ts) / (2.0*sigma + Ts);
}


//PID calculation
double PIDControl::PID(double y_r, double y){
    //Initialize variables to prevent compiler errors
    double u_unsat;

    //Compute the current error
    double error = y_r - y;

    //Integrate errkor using trapazoidal rule
    integrator = integrator + ((Ts/2) * (error + error_d1));

    //PID control
    if(flag == true){
        //Differentiate error
        error_dot = beta * error_dot + (((1 - beta)/Ts) * (error - error_d1));

        //PID control
        u_unsat = (kp*error) + (ki * integrator) + (kd * error_dot);
    }

    else{
        //differentiate y
        y_dot = beta * y_dot + (((1 - beta) / Ts) * (y - y_d1));

        //PID control
        u_unsat = (kp*error) + (ki * integrator) - (kd * y_dot);
    }

    //Return saturated control signal
    double u_sat = saturate(u_unsat);

    //Integrator anti-windup
    if(ki != 0.0){
        integrator = integrator + ((1.0 / ki) * (u_sat - u_unsat));
    }

    //Update delayed variables
    error_d1 = error;
    y_d1 = y;
    return u_sat;
}

//PD calculation
double PIDControl::PD(double y_r, double y){
//Initialize variables to prevent compiler errors
    double u_unsat;

    //Compute the current error
    double error = y_r - y;

    //Integrate errkor using trapazoidal rule
    integrator = integrator + ((Ts/2) * (error + error_d1));

    //PID control
    if(flag == true){
        //Differentiate error
        error_dot = beta * error_dot + (((1 - beta)/Ts) * (error - error_d1));

        //PID control
        u_unsat = (kp*error) + (kd * error_dot);
    }

    else{
        //differentiate y
        y_dot = beta * y_dot + (((1 - beta) / Ts) * (y - y_d1));

        //PID control
        u_unsat = (kp*error) - (kd * y_dot);
    }

    //Return saturated control signal
    double u_sat = saturate(u_unsat);

    //Update delayed variables
    error_d1 = error;
    y_d1 = y;
    return u_sat;
}

//Saturation check
double PIDControl::saturate(double u){
    //Check if absolute value is above limit, clip value if so
    if (abs(u) > limit){
        u = limit * (abs(u) / u);
    }
    return u;
}