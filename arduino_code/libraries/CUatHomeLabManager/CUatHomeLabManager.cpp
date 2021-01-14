/*
CUatHomeLabManager.cpp

YoungWoong Cho
Winter 2020

Function definitions for the CUatHomeLabManager class. This class manages the creation
and the destruction of the lab instances.
*/

#include <Arduino.h>
#include "CUatHomeLabManager.h"

CUatHomeLabManager::CUatHomeLabManager() {
	factory = new CUatHomeFactory();
	lab = factory->get_lab();
}
void CUatHomeLabManager::run(){
    lab->retrieve_cmd(); // Retrieve the command to check if the lab has changed
    if (lab->lab_changed) { // If lab changed, destroy the old lab and create the new lab instance
        int temp_lab_code{ lab->new_lab_code };
        factory->~CUatHomeFactory();
        lab->~CUatHomeLab();
        factory = new CUatHomeFactory(temp_lab_code);
        lab = factory->get_lab();
        lab->lab_code = temp_lab_code;
    }
    else lab->run_lab(); // If lab not changed, run the lab
}