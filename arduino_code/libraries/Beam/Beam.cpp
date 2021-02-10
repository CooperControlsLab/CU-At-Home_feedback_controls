/*
Beam.cpp

YoungWoong Cho
Winter 2020

Function definitions for the Beam class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Beam lab using the CUatHome kit.
*/

#include "Beam.h"
#include "CUatHomeLab.h"
#include <Arduino.h>


Beam::Beam(int ARDUINO_BOARD_CODE) {
	ARDUINO_CODE = ARDUINO_BOARD_CODE;
	Wire.begin();
	mpu6050->begin();
	continuous_send = true;
}

void Beam::DAQ() {}

void Beam::TSAQ() {
	//MPU6050
	mpu6050->update();

	// Serial Communication
	Serial.print('T'); Serial.print(current_micros);
	Serial.print(',');
	Serial.print("Angular acceleration: "); Serial.print(mpu6050->getAccAngleX());
	Serial.print(',');
	Serial.print("Translational acceleration: "); Serial.print(mpu6050->getAccX());
	Serial.print(',');
	Serial.print("Anglular displacement: "); Serial.println(mpu6050->getAngleX());
}