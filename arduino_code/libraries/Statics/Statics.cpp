/*
Statics.cpp

Jared Jacobowitz
Winter 2020

Function definitions for the Statics class. This class is a subclass of 
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Statics lab using the CUatHome kit.
*/

#include "Statics.h"
#include "CUatHomeLab.h"
#include <Arduino.h>


Statics::Statics() {
	pinMode(2, INPUT_PULLUP);
}


// nothing to process for this lab
void Statics::process_cmd() {
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

void Statics::run_lab() {
	current_micros = micros();
	delta = current_micros - prev_micros;
	if (write_data && delta >= dt * 1000000) {
		Serial.print('T'); Serial.print(current_micros - start_micros); 
		Serial.print(',');
		Serial.println(analogRead(A0));

		prev_micros = current_micros;
	}
}