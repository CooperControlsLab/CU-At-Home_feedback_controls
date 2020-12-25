/*
ProCon.cpp

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function definitions for the ProCon class. This class is a subclass of 
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the ProCon lab using the CUatHome kit.
*/

#include "ProCon.h"
#include "CUatHomeLab.h"
#include "PID_beard.h""
#include "Differentiator.h"
#include "motor_control_hardware_config.h"
#include <Arduino.h>

ProCon::ProCon() {
	double enc_count{ 0 };

	//Encoder Setup
	pinMode(ENC_A, INPUT_PULLUP);
	pinMode(ENC_B, INPUT_PULLUP);

	//attachInterrupt(digitalPinToInterrupt(ENC_A), pulseA, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENC_B), ProCon::pulseB, CHANGE);

	//Motor Setup
	pinMode(PWM_B, OUTPUT);
	pinMode(DIR_B, OUTPUT);
	digitalWrite(PWM_B, LOW); // Stop Motor on Start Up
}

void ProCon::process_cmd() {
	int pos;
	int cmd;

	//Handshake command
	cmd = get_cmd_code('H', -1);
	switch ((int)(cmd)) {
	case 0: // Handshake stuff to be implemented
		break;
	default: break;
	}

	//Request command
	cmd = get_cmd_code('R', -1);
	switch ((int)(cmd)) {
	case 0: //Flag to write data
		write_data = 1;
		break;

		//If no matches, break
	default: break;
	}

	//Set value/mode commands
	cmd = get_cmd_code('S', -1);
	switch (int(cmd)) {
	case 0: //Set PID gains
		kp = (double)(get_cmd_code('P', -1));
		ki = (double)(get_cmd_code('I', -1));
		kd = (double)(get_cmd_code('D', -1));
		break;

	case 1://Set Setpoint
		setpoint = (double)(get_cmd_code('Z', -1));
		break;

	case 2: //Set Lab type
		labType = (int)(get_cmd_code('Y', -1));
		break;

	case 3: //Set Controller Mode (on/off)
		mode = (int)(get_cmd_code('M', -1));
		break;

	case 4: //Set Sample Period
		double new_sample_period{ get_cmd_code('T', -1) };
		if (new_sample_period != -1) {
			sample_period = new_sample_period;
		}
		break;

	case 5: //Set Output Limits
		lowerOutputLimit = (double)(get_cmd_code('L', -1));
		upperOutputLimit = (double)(get_cmd_code('U', -1));
		break;

	case 6: //Openloop control
		open_loop_voltage = (double)(get_cmd_code('O', 0));
		break;

	case 7: //Feed Foward Voltage
		FF_A = (double)(get_cmd_code('A', 0));
		FF_B = (double)(get_cmd_code('B', 0));
		FF_C = (double)(get_cmd_code('C', 0));
		break;

	case 8: //Open Loop Step Resonse Analysis
		open_loop_analysis_start = true;
		open_loop_analysis_time = (double)(get_cmd_code('T', -1));
		break;

	case 9:
		calibration_start = true;
		break;

	case 10:
		anti_windup_activated = (int)(get_cmd_code('W', -1));
		break;

	default: break;
	}
}

void ProCon::run_lab() {
	return;
}

void ProCon::update_control_params() {
	//Check for OpenLoop Analysis and run
	if (open_loop_analysis_start)
	{
		//Create large array to store data
		double t0{ millis() };
		unsigned long init_time{ millis() };
		unsigned long current_time{ init_time };
		unsigned long prev_time{ init_time };

		//Set motor open loop voltage
		pid_output = open_loop_voltage;
		analogWrite(PWM_B, volts_to_PWM(pid_output));

		//initialize index variable
		int i{ 0 };

		//Reset Open Loop differentiator
		enc_deg = count_to_deg(enc_count);
		//diff.update_time_parameters(diff.Ts, 0.1);
		diff.reset(enc_deg);

		while (true) {
			enc_deg = count_to_deg(enc_count); //Update encoder degrees
			current_time = millis();

			//NB: replace millis with micros
			if (current_time - prev_time >= (diff.Ts * 1000)) {

				//append data to large array
				time[i] = current_time - init_time;
				velocity[i] = diff.differentiate(enc_deg);

				//If we're at the end of storage length, break the loop
				if (i >= (storage_length - 1)) {
					open_loop_analysis_start = false;
					diff.update_time_parameters(diff.Ts, 0.01); //Reset differentiator sigma value
					diff.reset(count_to_deg(enc_count));
					break;
				}

				prev_time = current_time; //Reset previous time
				i++; //Increment index counter
			}
		}

		// Send data function in SerialCommsBase?
		//Send long serial data to python
		for (int j = 0; j < storage_length; j++) {
			Serial.print("D0"); Serial.print(',');
			Serial.print('T'); Serial.print(time[j]); Serial.print(',');
			Serial.print('P'); Serial.print(0); Serial.print(',');
			Serial.print('V'); Serial.print(velocity[j] * DEGS_TO_RPM); Serial.print(',');
			Serial.print('I'); Serial.print(pid_output); Serial.print('$');
		}
		Serial.print('\n');
	}
}

void ProCon::update_motor_voltage(double voltage) {
	//Fix direction based on +/-
	if (voltage < 0) {
		digitalWrite(DIR_B, LOW); // CCW
	//    analogWrite(PWM_B, 0);
	//    return;
	}

	else {
		digitalWrite(DIR_B, HIGH); // CW
	}
	//set pwm based off of voltage
	analogWrite(PWM_B, volts_to_PWM(abs(voltage)));
}

int ProCon::volts_to_PWM(double voltage) {
	// Convert voltage to PWM duty cycle percent relative to the power supply voltage
	return constrain(round((voltage / SUPPLY_VOLTAGE) * 255), -255, 255);
}

double ProCon::PWM_to_volts(int PWM) {
	return double(PWM / 255) * SUPPLY_VOLTAGE;
}

double ProCon::pid_output_signal_conditioning(double val) {
	val = constrain(val, lowerOutputLimit, upperOutputLimit);
	return val;
}

double ProCon::count_to_deg(double count) {
	return (double(count / PPR) * 360);  // rad = (count / pulse/rev) * 360 deg/rev
}

void ProCon::pulseA() {
	int valA = digitalRead(ENC_A);
	int valB = digitalRead(ENC_B);

	if (valA == HIGH) { // A Rise
		if (valB == LOW) {
			enc_count++;  // CW
		}
		else {
			enc_count--;  // CCW
		}
	}
	else { // A fall
		if (valB == HIGH) {
			enc_count++;  // CW
		}
		else {
			enc_count--;  //CCW
		}
	}
}

static void ProCon::pulseB() {
	int valA = digitalRead(ENC_A);
	int valB = digitalRead(ENC_B);

	if (valB == HIGH) { // B rise
		if (valA == HIGH) {
			++enc_count; // CW
		}
		else {
			--enc_count; // CCW
		}
	}
	else { //B fall
		if (valA == LOW) {
			++enc_count; // CW
		}
		else {
			--enc_count; // CCW
		}
	}
}