/*
Statics.h

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
	unsigned long current_micros{ 0 };
	unsigned long prev_micros{ 0 };
	unsigned long start_micros{ 0 };

	double dt{ 0.0002 };
	unsigned long delta;

	int mic1;
	int mic2;

	bool write_data{ false };
public:
	SpeedofSound();
	void process_cmd();
	void run_lab();
};

#endif // !SPEED_OF_SOUND_H