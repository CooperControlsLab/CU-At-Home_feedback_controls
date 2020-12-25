#include "CUatHomeFactory.h"
#include "CUatHomeLab.h"

#include <motor_control_hardware_config.h>
#include <PID_beard.h>
#include <Differentiator.h>

//Timing Parameters for fixed interval calculations for PID and derivitave
//unsigned long prev_micros = 0;
//unsigned long current_micros;
//double sample_period = 0.005; //in sec

CUatHomeFactory *factory = new CUatHomeFactory(1);
CUatHomeLab *lab = factory->get_lab();

//***********************************************************************************
void setup() {
  Serial.begin(57600);
}

//***********************************************************************************
void loop() {
  lab->retrieve_cmd();
  if (lab->lab_changed){
    factory->~CUatHomeFactory();
//    lab->~CUatHomeLab();
    factory = new CUatHomeFactory(lab->lab_code);
    lab = factory->get_lab();
  }
  lab->run_lab();
}
