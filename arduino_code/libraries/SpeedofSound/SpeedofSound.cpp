/*
SpeedofSound.cpp

Jared Jacobowitz
Winter 2020

Function definitions for the SpeedofSound class. This class is a subclass of 
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Speed of Sound lab using the CUatHome kit.
*/

#include "SpeedofSound.h"
#include "CUatHomeLab.h"
#include <Arduino.h>


SpeedofSound::SpeedofSound() {
	Serial.begin(115200);
}


void SpeedofSound::process_cmd() {
	int cmd;

	cmd = get_cmd_code('R', -1);
	switch (cmd) {
	case 0: // toggle data writing
		write_data = !write_data;
		start_micros = micros();
		prev_micros = start_micros;
		break;
	default:
		break;
	}

	// Alternative form of this function
	//if (get_cmd_code('R', -1) != -1) {
	//	write_data = !write_data;
	//	start_micros = micros();
	//	prev_micros = start_micros;
	//}
}


void SpeedofSound::run_lab() {
	current_micros = micros();
	delta = current_micros - prev_micros;
	if (write_data && delta >= dt * 1000000) {
		// placed into variables to get their values at the proper time
		mic1 = analogRead(A0);
		mic2 = analogRead(A1);

		Serial.print('T'); Serial.print(current_micros - start_micros); 
		Serial.print(',');
		Serial.print(mic1); 
		Serial.print(',');
		Serial.println(mic2);
	}
}