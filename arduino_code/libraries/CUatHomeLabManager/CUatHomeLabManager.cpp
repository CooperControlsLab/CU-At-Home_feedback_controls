/*
CUatHomeLabManager.cpp

YoungWoong Cho
Winter 2020

Function definitions for the CUatHomeLabManager class. This class manages the 
creation and the destruction of the lab instances.
*/

#include <Arduino.h>
#include "CUatHomeLabManager.h"

CUatHomeLabManager::CUatHomeLabManager() {
	factory = new CUatHomeFactory();
	lab = factory->get_lab();
}
void CUatHomeLabManager::run(){
	// Retrieve the command to check if the lab has changed
    lab->retrieve_cmd();

	// If lab changed, destroy the old lab and create the new lab instance
    if (lab->lab_changed) { 
        int temp_lab_code{ lab->new_lab_code };
        factory->~CUatHomeFactory();
        lab->~CUatHomeLab();
        factory = new CUatHomeFactory(temp_lab_code);
        lab = factory->get_lab();
        lab->lab_code = temp_lab_code;
    }
    else lab->run_lab(); // If lab not changed, run the lab
}