/*
CUatHomeFactory.cpp

Jared Jacobowitz and YoungWoong Cho
Winter 2020

Function definitions for the CUatHomeFactory class. This class's sole purpose is
to create the proper lab when instantiated and then return that lab when called.

When adding a new lab to the CU@Home kit, the new lab must be added to the 
constructor switch statement and the header file must be included.
*/

#include "CUatHomeFactory.h"
#include "CUatHomeLab.h"
#include "ProCon.h"
<<<<<<< HEAD
#include "Statics.h"
#include "SpeedofSound.h"
#include "Beam.h"
=======
>>>>>>> cae0e6877a9d4c05ea318b8bb9aa09cb3d0b2e3d

// Constructor instantiates a CUatHomeLab object according to the specified lab
// code. This object will be dynamically allocated as there is no way to know
// which lab will be run before it is specified by the Python "L" command.
CUatHomeFactory::CUatHomeFactory(int lab_code) {
	switch (lab_code) {
	case 1:
		lab = new ProCon();
		break;
<<<<<<< HEAD
	case 2:
		lab = new Statics();
		break;
	case 3:
		lab = new SpeedofSound();
		break;
	case 4:
		lab = new Beam();
		break;
=======
>>>>>>> cae0e6877a9d4c05ea318b8bb9aa09cb3d0b2e3d
	default:
		//lab = nullptr;
		break;
	}
}

// Destructor called when changing the lab type (or the code finishes and the 
// object goes out of scope)
CUatHomeFactory::~CUatHomeFactory() {
	delete lab;
	lab = nullptr;
}

// lab is a pointer to the CUatHomeLab object, which will correspond to 
// instantiated lab type.
CUatHomeLab* CUatHomeFactory::get_lab() {
	return lab;
}