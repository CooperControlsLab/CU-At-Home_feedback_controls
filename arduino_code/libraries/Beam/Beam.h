/*
Beam.h

YoungWoong Cho
Winter 2020

Function declerations for the Beam class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Beam lab using the CUatHome kit.
*/

#ifndef BEAM_H
#define BEAM_H

#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b

#include "CUatHomeLab.h"
#include "Wire.h"
#include <Arduino.h>

class Beam : public CUatHomeLab {
//MPU6050
private:
	TwoWire* wire;
	int16_t rawAccX, rawAccY, rawAccZ, rawGyroX, rawGyroY, rawGyroZ;
	float accX, accY, accZ, gyroX, gyroY, gyroZ;
	float angleAccX, angleAccY, angleAccZ, angleGyroX, angleGyroY, angleGyroZ;
	float angleX, angleY, angleZ;
	float interval;
	long preInterval;
	float accCoef, gyroCoef;
public:
	// Constructor
	MPU6050(TwoWire& w);
	// Raw data
	int16_t getRawAccX() { return rawAccX; };
	int16_t getRawAccY() { return rawAccY; };
	int16_t getRawAccZ() { return rawAccZ; };
	int16_t getRawGyroX() { return rawGyroX; };
	int16_t getRawGyroY() { return rawGyroY; };
	int16_t getRawGyroZ() { return rawGyroZ; };
	// Acceleration & Gyro
	float getAccX() { return accX; };
	float getAccY() { return accY; };
	float getAccZ() { return accZ; };
	float getGyroX() { return gyroX; };
	float getGyroY() { return gyroY; };
	float getGyroZ() { return gyroZ; };
	// Accel & Gyro angles
	float getAccAngleX() { return angleAccX; };
	float getAccAngleY() { return angleAccY; };
	float getAccAngleZ() { return angleAccZ; };
	float getGyroAngleX() { return angleGyroX; };
	float getGyroAngleY() { return angleGyroY; };
	float getGyroAngleZ() { return angleGyroZ; };
	// Angles
	float getAngleX() { return angleX; };
	float getAngleY() { return angleY; };
	float getAngleZ() { return angleZ; };

	// Serial
	void writeMPU6050(byte reg, byte data);

// Beam Lab
private:
	unsigned long current_micros{ 0 };
	unsigned long prev_micros{ 0 };
	unsigned long start_micros{ 0 };

	double dt{ 0.0001 };
	unsigned long delta;

	bool write_data{ false };

	int labtype;
public:
	Beam();
	void process_cmd();
	void run_lab();
};

#endif // !BEAM_H