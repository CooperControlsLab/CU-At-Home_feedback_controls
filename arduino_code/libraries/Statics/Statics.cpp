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


Statics::Statics(int ARDUINO_BOARD_CODE) { 
	ARDUINO_CODE = ARDUINO_BOARD_CODE;
	pinMode(2, INPUT_PULLUP);
	continuous_send = true;
}


void Statics::DAQ() {}

void Statics::TSAQ() {
	Serial.print('T'); Serial.print(current_micros);
	Serial.print(',');
	Serial.print('S'); Serial.print(100);
	Serial.print(',');
	Serial.print('A'); Serial.println(analogRead(A0));
}