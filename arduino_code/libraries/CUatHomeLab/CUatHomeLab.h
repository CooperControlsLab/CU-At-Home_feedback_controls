/*
CUatHomeLab.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function declerations for the abstract CUatHomeLab class. All labs will inherit 
this class (and by extension the inherited super-class SerialComms). This class 
outlines that each lab will have at least two functions: process_cmd and 
run_lab. These are virtual functions which will be defined by each lab.

ARDUINO_CODE: TEENSY = 0, UNO = 1
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
	int data_array_length{ 0 };  // board-type dependent

	/* 2)	For sampling */
	unsigned long current_micros{ 0 }; // current time in the experiment
	unsigned long prev_micros{ 0 };  // track elapsed time from last data send
	unsigned long start_micros{ 0 };  // offset time for experiment
	unsigned long time{ 0 }; // [s]; for discontinuous send; elapsed time 
							 // approximately constant so just increment by dt
	
	double dt{ 0.01 }; // Sample time interval [s]
	double new_dt{ 0.01 }; // New sampling time interval
	
	double sample_time{ 100 }; // Total sample time [s]
	double new_sample_time{ 100 }; // New sample time [s]
	
	unsigned long delta; // Time passed since last prev_micros was logged [s]
	
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
	void process_cmd();

	/* 5)	This function is called each loop in the arduino code. */
	void run_lab();

	/* 6)	For data logging */
	virtual void DAQ() = 0;
	bool log_data{ false }; // true when experiment starts. false when 1) log_index catches up send_index 2) STOP button pressed
	int log_index; // index that will be logged to the array
	bool wrap{ false };  // true when the log_index loops back to 0
	bool started_experiment{ false };  // true when first R1 is recieved

	/* 7)	For data sending */
	virtual void TSAQ() = 0;
	bool send_data{ false };
	int send_index; // index that will be sent to serial
	bool continuous_send{ false };

	CUatHomeLab();
};

#endif // !CU_AT_HOME_H