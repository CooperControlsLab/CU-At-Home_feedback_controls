/*
GeneralDAQ.h

YoungWoong Cho
Winter 2020

Function definitions for the GeneralDAQ class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the GeneralDAQ lab using the CUatHome kit.
*/

#include "GeneralDAQ.h"
#include "CUatHomeLab.h"
#include <Arduino.h>


GeneralDAQ::GeneralDAQ(int ARDUINO_BOARD_CODE) {
	ARDUINO_CODE = ARDUINO_BOARD_CODE;
	dt = 0.01;
	continuous_send = true;
}

void GeneralDAQ::TSAQ() {
	analog0 = analogRead(A0);
	analog1 = analogRead(A1);
	analog2 = analogRead(A2);
	analog3 = analogRead(A3);
	analog4 = analogRead(A4);

	Serial.print('T'); Serial.print(current_micros);
	Serial.print(',');
	Serial.print("A0: "); Serial.print(analog0);
	Serial.print(',');
	Serial.print("A1: "); Serial.print(analog1);
	Serial.print(',');
	Serial.print("A2: "); Serial.print(analog2);
	Serial.print(',');
	Serial.print("A3: "); Serial.print(analog3);
	Serial.print(',');
	Serial.print("A4: "); Serial.print(analog4);
	Serial.print('\n');
}

void GeneralDAQ::DAQ() {}