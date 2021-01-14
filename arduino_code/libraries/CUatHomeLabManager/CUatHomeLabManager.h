/*
CUatHomeLabManager.h

YoungWoong Cho
Winter 2020

Function declerations for the CUatHomeLabManager class. This class manages the creation
and the destruction of the lab instances.
*/

#ifndef CU_AT_HOME_LABMANAGER_H
#define CU_AT_HOME_LABMANAGER_H

#include <Arduino.h>
#include "CUatHomeFactory.h"
#include "CUatHomeLab.h"

class CUatHomeLabManager {
public:
	CUatHomeLabManager();
	void run();
private:
	CUatHomeFactory* factory;
	CUatHomeLab *lab;
};

#endif // !CU_AT_HOME_LABMANAGER_H