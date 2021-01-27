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

SpeedofSound::SpeedofSound(int ARDUINO_BOARD_CODE) {
	ARDUINO_CODE = ARDUINO_BOARD_CODE;

	// Test different array sizes with arduinos. >250 doesn't work on Uno
	// ARDUINO_CODE: TEENSY = 0, UNO = 1
	if (ARDUINO_CODE == 0) data_array_length = 500;
	else if (ARDUINO_CODE == 1) data_array_length = 250;

	mic1 = new int[data_array_length];
	mic2 = new int[data_array_length];
}

SpeedofSound::~SpeedofSound() {
	delete[] mic1;
	delete[] mic2;
}

void SpeedofSound::process_cmd() {
	int cmd;

	cmd = get_cmd_code('R', -1);
	switch (cmd) {
	case 0: // end experiment
		log_data = false;
		started_experiment = false;
		break;
	case 1: // start experiment if not already; toggle write_data to print data
		if (!started_experiment) {
			started_experiment = true;
			log_data = true;
			wrap = false;
			
			time = 0;
			print_index = 0;
			write_index = 0;
			start_micros = micros();
			prev_micros = start_micros;
		}
		write_data = true;
		break;
	default:
		break;
	}
}

void SpeedofSound::run_lab() {
	current_micros = micros();
	delta = current_micros - prev_micros;

	// print data if ALL:
	// 1. write_data = true, i.e. data has been requested
	// 2. dt has passed
	// 3. won't be printing data it has already printed after it stopped logging
	//    (it will reprint if wrap is false (meaning print_index is on the same
	//	  loop around as write_index) AND print_index does not lag by 1 index)
	if (write_data && delta >= dt * 1000000 
		&& (log_data || wrap || print_index < write_index - 1)) {
		Serial.print('T'); Serial.print(time);
		time += dt * 1000000;
		Serial.print(',');
		Serial.print('S'); Serial.print(100);
		Serial.print(',');
		Serial.print('A'); Serial.print(mic1[print_index]);
		Serial.print(',');
		Serial.print('Q'); Serial.println(mic2[print_index]);

		++print_index;
		print_index %= data_array_length;
		if (print_index == 0) wrap = false;

		write_data = false;

		//Serial.print("Print Index: "); Serial.println(print_index);
	}
	if (log_data && delta >= dt * 1000000) {
		// if true, it will be overwriting data that hasn't been sent
		if (wrap && write_index == print_index) {
			log_data = false;
			//Serial.println("Stopped logging.");
		}
		else {
			mic1[write_index] = analogRead(A0);
			mic2[write_index] = analogRead(A1);

			++write_index;
			write_index %= data_array_length;
			if (write_index == 0) wrap = true ;

			//Serial.print("Write Index: "); Serial.println(write_index);
			//Serial.print("Wrap: "); Serial.println(wrap);
		}
	}
}