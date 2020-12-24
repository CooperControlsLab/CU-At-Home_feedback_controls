/*
CUatHomeLab.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function definitions for the abstract CUatHomeLab class. All labs will inherit
this class (and by extension the inherited super-class SerialComms). This class
outlines that each lab will have at least two functions: process_cmd and
run_lab. These are virtual functions which will be defined by each lab.
*/

#include "CUatHomeLab.h"
#include "SerialCommsBase.h"
#include <Arduino.h>

CUatHomeLab::CUatHomeLab() {}

// Each lab will interpret the commands differently. This function will
// be implemented by each lab, looking for specific command codes and
// then calling the proper functions.
void CUatHomeLab::process_cmd() {
	return;
}

// This function is called each loop in the arduino code.
void CUatHomeLab::run_lab() {
	return;
}