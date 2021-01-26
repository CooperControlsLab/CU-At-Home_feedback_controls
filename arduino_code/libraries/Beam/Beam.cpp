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
}

void Beam::process_cmd() {
	int cmd;
	cmd = get_cmd_code('R', -1);
	switch (cmd) {
	case 0: // toggle data writing off
		write_data = false;
		break;
	case 1: // toggle data writing on
		if (!write_data) {
			write_data = true;
			start_micros = micros();
			prev_micros = start_micros;
			time = 0;
		}
		break;
	default:
		break;
	}
}

void Beam::run_lab() {
	// Updates time variable
	current_micros = micros();
	delta = current_micros - prev_micros;

	// Sends data if ALL:
	// 1. write_data is true
	// 2. dt has passed
	// 3. sample_time has not passed
	if (write_data && delta >= dt * 1000000) {
		if (time <= sample_time * 1000000){
			//MPU6050
			mpu6050->update();

			// Serial Communication
			Serial.print('T'); Serial.print(time);
			Serial.print(',');
			Serial.print("S"); Serial.print(mpu6050->getAccAngleX());
			Serial.print(',');
			Serial.print("A"); Serial.print(mpu6050->getAccX());
			Serial.print(',');
			Serial.print("Q"); Serial.println(mpu6050->getAngleX());

			time += dt * 1000000;
			prev_micros = current_micros;
		}
		else if (write_data){
			//Serial.println("Tell python I'm done with my time!");
			write_data = false;
		}
	}
}