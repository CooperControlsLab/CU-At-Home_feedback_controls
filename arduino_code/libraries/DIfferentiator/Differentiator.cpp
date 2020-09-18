#include <Differentiator.h>

Differentiator::Differentiator(double sig, double t_rate){
    sigma = sig;
    Ts = t_rate;
    beta = (2.0*sigma - Ts) / (2.0*sigma + Ts);
    y_dot = 0;
}

double Differentiator::differentiate(double y){

    // calculate derivative
    y_dot = (beta * y_dot) + (((1 - beta)/Ts) * (y - y_d1));

    //set y_d1 to current val
    y_d1 = y;

    //return derivative value
    return y_dot;

}