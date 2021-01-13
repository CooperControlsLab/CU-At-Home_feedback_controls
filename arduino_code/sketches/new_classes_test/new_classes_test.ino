#include "CUatHomeFactory.h"
#include "CUatHomeLab.h"
#include "Statics.h"

#include <motor_control_hardware_config.h>
#include <PID_beard.h>
#include <Differentiator.h>

//GeneralDAQ = 1, Statics = 2, Speed of Sound = 3, Beam = 4
CUatHomeFactory *factory = new CUatHomeFactory(1);
CUatHomeLab *lab = factory->get_lab();

//***********************************************************************************
void setup() {
  Serial.begin(500000);
}

//***********************************************************************************
void loop() {
  lab->retrieve_cmd(); // Retrieve the command to check if the lab has changed
  if (lab->lab_changed){ // If lab changed, destroy the old lab and create the new lab instance
    int temp_lab_code{ lab->new_lab_code };
    //Serial.print("Changing Lab to LAB "); Serial.println(temp_lab_code);
    factory->~CUatHomeFactory();
    lab->~CUatHomeLab();
    factory = new CUatHomeFactory(temp_lab_code);
    lab = factory->get_lab();
    lab->lab_code = temp_lab_code;
  }
  else lab->run_lab(); // If lab not changed, run the lab
}
