/*
SpeedofSound.cpp

Jared Jacobowitz and YoungWoong Cho
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
		write_data = false;
		break;
	case 1: // start experiment if not already; toggle write_data to print data
		if (!started_experiment) {
			started_experiment = true;
			log_data = true;
			wrap = false;

			time = 0;
			print_index = 0;
			log_index = 0;
			start_micros = micros();
			prev_micros = start_micros;
		}
		write_data = true;
		break;
	default:
		break;
	}
}

// run_lab method must take control of the follows:
// With the assumption: log_index increases faster than print_index does
// 1. keep logging data to the array as fast as possible
// 2. keep sending data to the serial as fast as possible
// 3. reset the indices log_index and print_index to 0 the end of the array is reached
// 4. if log_index catches up print_index, stop logging while keep printing
// 5. if print_index catches up log_index, begin logging while keep printing
void SpeedofSound::run_lab() {
	// Updates time variable
	current_micros = micros();
	delta = current_micros - prev_micros;

	// 1. Checks if DAQ is ready
	// if ALL:
	// 	a) write_data is true; i.e. when the data is requested
	//	b) dt has passed
	//	c) sample_time has not passed
	if (write_data && delta >= dt * 1000000){
		if (time <= sample_time * 1000000){
			// 2. Checks if log_data is true; if so, log data
			// if (log_data){
			// 	mic1[log_index] = analogRead(A0);
			// 	mic2[log_index] = analogRead(A1);

			// 	// If end of the array is reached, reset log_index to 0 and set wrap = true
			// 	log_index++;
			// 	log_index %= data_array_length;
			// 	if (log_index == 0 && !wrap) wrap = true;
			// }


			// // 3. Checks if log_index = print_index
			// // If this is the case, log_index has catched up print_index
			// if (log_index == print_index){
			// 	// 3-1. Checks if log_index has catched up print_index; if so, stop logging while keep printing
			// 		if (wrap){ log_data = false; wrap = false;}
			// 	// // 3-2. Checks if print_index has catched up log_index; if so, begin logging while keep printing
			// 	// 	else { log_data = true;}
			// }

			// 4. Whether or not log_data is true, keep sending data to the serial
			//Serial.print("Print Index: "); Serial.println(print_index);
			Serial.print('T'); Serial.print(time);
			Serial.print(',');
			Serial.print('S'); Serial.print(100);
			Serial.print(',');
			Serial.print('A'); Serial.print(mic1[print_index]);
			Serial.print(',');
			Serial.print('Q'); Serial.println(mic2[print_index]);

			// If end of the array is reached, reset print_index to 0
			print_index++;
			print_index %= data_array_length;
			
			time += dt * 1000000;
			prev_micros = current_micros;
			//write_data = false;
		}
		else if (write_data){
			//Serial.println("Tell python I'm done with my time!");
			write_data = false;
			started_experiment = false;
		}
	}
}