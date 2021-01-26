/*
CUatHomeLab.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function declerations for the abstract CUatHomeLab class. All labs will inherit 
this class (and by extension the inherited super-class SerialComms). This class 
outlines that each lab will have at least two functions: process_cmd and 
run_lab. These are virtual functions which will be defined by each lab.
*/

#ifndef CU_AT_HOME_H
#define CU_AT_HOME_H

#include <Arduino.h>

class CUatHomeLab {
public:
	/*** Lab ***/
	/* 1)	For changing between labs */
	int lab_code; // Current lab code
	int new_lab_code;
	bool lab_changed{ false };  // Will become true when new lab command
	int ARDUINO_CODE;  // 0:TEENSY, 1:UNO

	/* 2)	For sampling */
	unsigned long current_micros{ 0 };
	unsigned long prev_micros{ 0 };
	unsigned long start_micros{ 0 };
	double time{ 0 }; // Sampling time [s]; Time between data points will be approximately constant so this variable will just be incremented each print
	double dt; // Sampling time interval [s]
	double new_dt; // New sampling time interval
	double sample_time; // Total sample time [s]
	double new_sample_time; // New sample time [s]
	unsigned long delta; // Time passed since last prev_micros is logged [s]

	bool write_data{ false };
	
	/*** Serial communication ***/
	/* 1)	For command retrieval */
	static const int storage_length{ 200 };
	char cmd_string[storage_length];
	int cmd_index{ 0 };
	char incoming_char;
	bool cmd_retrieved{ false };

	/* 2)	Reads a single command
			Called by lab to retrieve command string */
	void retrieve_cmd();

	/* 3)	Search cmd_string for the number after the specifiec key
			Returns the number immediately after key if it exists, def otherwise */
	double get_cmd_code(char key, int def);


	/* 4)	Each lab will interpret the commands differently. This function will
			be implemented by each lab, looking for specific command codes and
			then calling the proper functions. */
	virtual void process_cmd() = 0;

	/* 5)	This function is called each loop in the arduino code. */
	virtual void run_lab() = 0;

	CUatHomeLab();
};

#endif // !CU_AT_HOME_H