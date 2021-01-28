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
	lab_code = 3;
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

void SpeedofSound::DAQ(){
	mic1[log_index] = analogRead(A0);
	mic2[log_index] = analogRead(A1);
}

void SpeedofSound::TSAQ() {
	Serial.print('T'); Serial.print(time);
	Serial.print(',');
	Serial.print('S'); Serial.print(log_index);
	Serial.print(',');
	Serial.print('A'); Serial.print(send_index);
	Serial.print(',');
	Serial.print('Q'); Serial.println(is_sending_without_log_data);
}