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

#include <Wire.h>
#include <Arduino.h>
#include "CUatHomeLab.h"
#include "MPU6050.h"

class Beam : public CUatHomeLab {
//MPU6050
private:
	MPU6050* mpu6050 = new MPU6050(Wire);

// Beam Lab
public:
	Beam();
	void process_cmd();
	void run_lab();
};

#endif // !BEAM_H