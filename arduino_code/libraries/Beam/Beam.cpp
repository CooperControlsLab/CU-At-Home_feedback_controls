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
	writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);
	writeMPU6050(MPU6050_CONFIG, 0x00);
	writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);
	writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00);
	writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);
	angleGyroX = 0;
	angleGyroY = 0;
	angleX = this->getAccAngleX();
	angleY = this->getAccAngleY();
}

void Beam::writeMPU6050(byte reg, byte data) {
	wire->beginTransmission(MPU6050_ADDR);
	wire->write(reg);
	wire->write(data);
	wire->endTransmission();
}

void Beam::process_cmd() {
	int cmd;
	cmd = get_cmd_code('R', -1);
	switch (cmd) {
	case 0: // toggle data writing
		write_data = !write_data;
		start_micros = micros();
		prev_micros = start_micros;
		break;
	default:
		break;
	}
}

void Beam::run_lab() {
	current_micros = micros();
	delta = current_micros - prev_micros;
	if (write_data && delta >= dt * 1000000) {

		//MPU6050
		wire->beginTransmission(MPU6050_ADDR);
		wire->write(0x3B);
		wire->endTransmission(false);
		wire->requestFrom((int)MPU6050_ADDR, 14);

		rawAccX = wire->read() << 8 | wire->read();
		rawAccY = wire->read() << 8 | wire->read();
		rawAccZ = wire->read() << 8 | wire->read();
		rawGyroX = wire->read() << 8 | wire->read();
		rawGyroY = wire->read() << 8 | wire->read();
		rawGyroZ = wire->read() << 8 | wire->read();

		accX = ((float)rawAccX) / 16384.0;
		accY = ((float)rawAccY) / 16384.0;
		accZ = ((float)rawAccZ) / 16384.0;

		angleAccX = atan2(accY, accZ + abs(accX)) * 360 / 2.0 / PI;
		angleAccY = atan2(accX, accZ + abs(accY)) * 360 / -2.0 / PI;

		gyroX = ((float)rawGyroX) / 65.5;
		gyroY = ((float)rawGyroY) / 65.5;
		gyroZ = ((float)rawGyroZ) / 65.5;

		/*gyroX -= gyroXoffset;
		gyroY -= gyroYoffset;
		gyroZ -= gyroZoffset;*/

		angleGyroX += gyroX * delta;
		angleGyroY += gyroY * delta;
		angleGyroZ += gyroZ * delta;

		angleX = (gyroCoef * (angleX + gyroX * delta)) + (accCoef * angleAccX);
		angleY = (gyroCoef * (angleY + gyroY * delta)) + (accCoef * angleAccY);
		angleZ = angleGyroZ;


		// Serial Communication
		Serial.print('T'); Serial.print(current_micros - start_micros);
		Serial.print(',');
		Serial.print('Angular acceleration'); Serial.print(angleAccX);
		Serial.print(',');
		Serial.print('Translational acceleration'); Serial.print(accX);
		Serial.print(',');
		Serial.print('Anglular displacement'); Serial.println(angleX);

		prev_micros = current_micros;
	}
}