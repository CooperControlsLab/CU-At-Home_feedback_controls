/*
CUatHomeLab.h

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function declerations for the abstract CUatHomeLab class. All labs will inherit 
this class (and by extension the inherited super-class SerialComms). This class 
outlines that each lab will have at least two functions: process_cmd and 
run_lab. These are virtual functions which will be defined by each lab.
*/

#ifndef _CU_AT_HOME_H
#define _CU_AT_HOME_H

#include "SerialCommsBase.h"
#include <Arduino.h>

// This class will contain the two functions required for all labs, those 
// functions are: process_cmd and run_lab
class CUatHomeLab : public SerialComms {
public:
	CUatHomeLab();
	
	// Each lab will interpret the commands differently. This function will
	// be implemented by each lab, looking for specific command codes and
	// then calling the proper functions.
	virtual void process_cmd();

	// This function is called each loop in the arduino code.
	virtual void run_lab();
};

#endif // _CU_AT_HOME_H