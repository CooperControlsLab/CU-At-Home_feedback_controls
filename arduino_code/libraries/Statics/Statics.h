/*
Statics.h

Jared Jacobowitz
Winter 2020

Function declerations for the Statics class. This class is a subclass of
CUatHomeLab and therefore must implement the process_cmd and run_lab functions.
This class contains also contains all the variables and functions necessary for
running the Statics lab using the CUatHome kit.
*/

#ifndef STATICS_H
#define STATICS_H

#include "CUatHomeLab.h"
#include <Arduino.h>

class Statics : public CUatHomeLab {
public:
	Statics();
	void process_cmd();
	void run_lab();
};

#endif // !STATICS_H