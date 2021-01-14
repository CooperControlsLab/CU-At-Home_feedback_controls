/*
CUatHomeLab.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function definitions for the abstract CUatHomeLab class. All labs will inherit
this class (and by extension the inherited super-class SerialComms). This class
outlines that each lab will have at least two functions: process_cmd and
run_lab. These are virtual functions which will be defined by each lab.
*/

#include "CUatHomeLab.h"
//#include "SerialCommsBase.h"
#include <Arduino.h>

CUatHomeLab::CUatHomeLab() {};

// Each lab will interpret the commands differently. This function will
// be implemented by each lab, looking for specific command codes and
// then calling the proper functions.
void CUatHomeLab::retrieve_cmd() {
	if (Serial.available()) {
		// Store a single command into the cmd_string array
		incoming_char = Serial.read();
		cmd_string[cmd_index] = incoming_char;
		// If end of the command, process the command
		if (incoming_char == '\0' || incoming_char == '%') {
			new_lab_code = get_cmd_code('L', -1);
			// If lab changed
			if (new_lab_code != -1 && new_lab_code != lab_code) {
				lab_changed = true;

				Serial.print("Previous lab: "); Serial.println(lab_code);
				Serial.print("New lab: "); Serial.println(new_lab_code);

			}
			// If lab not changed
			else {
				process_cmd();
			}
			cmd_index = 0;
			memset(cmd_string, '\0', sizeof(cmd_string)); // resets to all \0
		}
		// If not end of the command, proceed to the next char
		else {
			++cmd_index;
		}
	}
}

double CUatHomeLab::get_cmd_code(char key, int def) {
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

	// Find position of the comma or \0 to get the index of the end of the code.
	// % is for manual testing in the Arduino serial monitor.
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