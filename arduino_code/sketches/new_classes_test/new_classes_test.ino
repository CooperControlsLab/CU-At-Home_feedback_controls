#include "CUatHomeLabManager.h"

//DefaultLab = 0, GeneralDAQ = 1, Statics = 2, Speed of Sound = 3, Beam = 4
CUatHomeLabManager *labMgr = new CUatHomeLabManager();

void setup() { Serial.begin(500000); }

void loop() { labMgr->run(); }
