/*
SerialCommsBase.cpp

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Abstract class for serial communications. This class contains all the functions
necessary for retrieving strings, parsing commands, and sending data. This class
is inherited by CUatHomeLab class to be used for serial communication purposes.
This cpp file contains the function definitions.
*/

#include "SerialCommsBase.h"
#include <string.h>
#include <Arduino.h>

SerialComms::SerialComms(){}

//// Updates the sample rate
//void SerialComms::update_sample_period(double new_sample_period) {
//	sample_period = new_sample_period;
//}

// Search cmd_string for key, return the number between key and delimiter
// Returns number if the command exists, "def" if it does not exist
double SerialComms::get_cmd_code(char key, int def) {
	//Serial.print("Key: "); Serial.println(key);

	// Search string for first instance of key; return def if not found
	int cmd_start{ 0 };
	while (cmd_string[cmd_start]) {
		if (cmd_string[cmd_start] == key) {
			++cmd_start; // increment to get the index after the key
			break;
		}
		if (cmd_string[cmd_start + 1] == '\0' || cmd_string[cmd_start + 1] == '%') {
			return def;
		}
		++cmd_start;
	}

	// Find position of the comma or \0 to get the index of the end of the code
	int cmd_end{ cmd_start };
	while (cmd_string[cmd_end] && !(cmd_string[cmd_end] == ',' || cmd_string[cmd_end] == '\0' || cmd_string[cmd_end] == '%')) {
		++cmd_end;
	}

	//Serial.print("Start: "); Serial.println(cmd_start);
	//Serial.print("End: "); Serial.println(cmd_end);
	char cmd[20];
	strncpy(cmd, &cmd_string[cmd_start], cmd_end - cmd_start);
	cmd[cmd_end - cmd_start] = '\0'; // dangerous if code is too long
	//Serial.print("cmd array: "); Serial.println(cmd);
	return atof(cmd); // return the substring in float format
}

// Extracts the cmd_string from the Ardunio Serial
// This
void SerialComms::retrieve_cmd() {
	if (Serial.available()) {
		//Serial.println(Serial.available());
		incoming_char = Serial.read();
		cmd_string[cmd_index] = incoming_char;
		if (incoming_char == '\0' || incoming_char == '%') {
			//Serial.println("End of line, processing commands!");
			//Serial.print("CMD: "); Serial.println(cmd_string);
			cmd_index = 0;
			if (cmd_string[0] == 'L') {
				lab_code = get_cmd_code('L', -1);
				lab_changed = true;
			}
			else {
				process_cmd();
			}
			memset(cmd_string, '\0', sizeof(cmd_string)); // necessary?
		}
		else {
			++cmd_index;
		}
	}
}

// Prints the data to the Serial monitor for Python
void SerialComms::send_data(char* data) {
	Serial.println(data);
}