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


Beam::Beam() {
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
		if (write_data == false) {
			write_data = true;
			start_micros = micros();
			prev_micros = start_micros;
			break;
		}
		else { break; }
	default:
		break;
	}
}

void Beam::run_lab() {
	current_micros = micros();
	delta = current_micros - prev_micros;
	if (write_data && delta >= dt * 1000000) {
		//MPU6050
		mpu6050->update();

		// Serial Communication
		Serial.print('T'); Serial.print(current_micros - start_micros);
		Serial.print(',');
		Serial.print("Angular acceleration: "); Serial.print(mpu6050->getAccAngleX());
		Serial.print(',');
		Serial.print("Translational acceleration: "); Serial.print(mpu6050->getAccX());
		Serial.print(',');
		Serial.print("Anglular displacement: "); Serial.print(mpu6050->getAngleX());
		Serial.print('\n');

		prev_micros = current_micros;
	}
}