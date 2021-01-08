/*
SerialCommsBase.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Abstract class for serial communications. This class contains all the functions
necessary for retrieving strings, parsing commands, and sending data. This class
is inherited by CUatHomeLab class to be used for serial communication purposes.
This header file contains the function declerations.
*/

#ifndef SERIAL_COMMS_BASE_H
#define SERIAL_COMMS_BASE_H

#include <Arduino.h>

class SerialComms {
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

	// S0,P0.1,I0,D0,%
	// S1,Z100,%
	// S2,Y0,%
	// S3,M1,%
	// S4,T0.005,%
	// S5,L-12,U12,%

public:
	double sample_period{ 0.005 };
	int lab_code;
	int new_lab_code;
	bool lab_changed{ false };  // Start true and then overwrite for new lab

	// For data processing
	static const int storage_length{ 200 };
	char cmd_string[storage_length];
	int cmd_index{ 0 };
	char incoming_char;
	bool cmd_retrieved{ false };


	// Instantiate SerialComms for use in extracting lab selection
	SerialComms();

	// Updates the sample rate
	//void update_sample_period(double new_sample_period);

	// Search cmd_string for the number after the specifiec key
	// Returns number immediately after key if it exists, def otherwise
	double get_cmd_code(char key, int def);

	// Called by lab to retrieve command string
	void retrieve_cmd();

	// Process a command received from serial buffer
	// virtual function implemented in CUatHomeLab; switch per lab
	virtual void process_cmd();

	// Sends requested data; will need to change input type that makes sense for all
	void send_data(char* data);
};

#endif // !SERIAL_COMMS_BASE_H 