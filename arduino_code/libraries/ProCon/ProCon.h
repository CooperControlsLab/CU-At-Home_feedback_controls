/*
ProCon.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function declerations for the ProCon class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the ProCon lab using the CUatHome kit.
*/

#include "CUatHomeLab.h"
#include <PID_beard.h>
#include <Differentiator.h>
#include <motor_control_hardware_config.h>
#include <Arduino.h>

class ProCon : public CUatHomeLab {
private:
	double kp, ki, kd;
	double setpoint;

	double prev_deg;
	double lowerLimit;
	double upperLimit;

	int mode;
	int lowerOutputLimit;
	int upperOutputLimit;

	int write_data;
	char data[200];

	int labType;

	int open_loop_voltage;

	double feed_forward_voltage;
	double FF_A;
	double FF_B;
	double FF_C;

	bool open_loop_analysis_start;
	double open_loop_analysis_time;

	bool calibration_start;
	int anti_windup_activated;

	double sigma;

	double sample_period; //in sec
	bool flag;
	double pid_output;
	bool controller_on;

	double current_micros;
	double prev_micros;

	double new_sample_period;

	double enc_deg;
	double enc_count;

public:
	ProCon();
	void process_cmd();
	void run_lab();

	void update_control_params();
	void compute_motor_voltage();
	void update_motor_voltage(double voltage);
	int volts_to_PWM(double voltage);
	double PWM_to_volts(int PWM);
	double pid_output_signal_conditioning(double val);
	double count_to_deg(double count);
	void pulseA();
	void pulseB();
};