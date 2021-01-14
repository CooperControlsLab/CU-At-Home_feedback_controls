/*
CUatHomeLabManager.h

YoungWoong Cho
Winter 2020

Function declerations for the CUatHomeLabManager class. This class manages the creation
and the destruction of the lab instances.
*/

#ifndef CU_AT_HOME_LABMANAGER_H
#define CU_AT_HOME_LABMANAGER_H

#include "CUatHomeFactory.h"
#include "CUatHomeLab.h"

#include <Arduino.h>

class CUatHomeLabManager {
private:
	CUatHomeFactory* factory;
	CUatHomeLab* lab;
public:
	CUatHomeLabManager();
	void run();
};

#endif // !CU_AT_HOME_LABMANAGER_H