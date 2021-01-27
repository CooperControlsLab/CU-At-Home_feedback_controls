/*
SpeedofSound.h

Jared Jacobowitz
Winter 2020

Function declerations for the SpeedofSound class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Speed of Sound lab using the CUatHome kit.
*/

#ifndef SPEED_OF_SOUND_H
#define SPEED_OF_SOUND_H

#include "CUatHomeLab.h"
#include <Arduino.h>

class SpeedofSound : public CUatHomeLab {
private:
	// time between data points will be approximately constant so this variable
	// will just be incremented each print
	unsigned long time; 
	bool started_experiment{ false };  // true when first R1 is recieved
	bool log_data{ false };
	int data_array_length{ 0 };  // board-type dependent
	int print_index;
	int write_index;
	bool wrap;  // true if write_index has looped back and print_index has not
	float t{ 0 };

	// Dynamically allocated arrays for data holding.
	// Need to be dynamic because the array size depends on the Arduino.
	int* mic1;
	int* mic2;
public:
	SpeedofSound(int ARDUINO_BOARD_CODE);
	~SpeedofSound();
	void process_cmd();
	void run_lab();
};

#endif // !SPEED_OF_SOUND_H