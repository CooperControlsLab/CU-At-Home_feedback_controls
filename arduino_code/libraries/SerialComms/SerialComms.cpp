#include <SerialComms.h>
#include <string.h>
#include <Arduino.h>

SerialComms::SerialComms(){
    //Initialize Serial buffer params
    cmd_index=0;

    setpoint = 0;
    labType = 0;
    mode = 0;
    lowerOutputLimit = -125;
    upperOutputLimit = 125;
    sampleTime = 0.005;
    kp = 0;
    ki = 0;
    kd = 0;
    write_data = 0;
    open_loop_voltage = 0;
    FF_A = 0;
    FF_B = 0;
    FF_C = 0;

    calibration_start = false;
    open_loop_analysis_start = false;
}

void SerialComms::process_command(char* cmd_string){
    int pos;
    int cmd;

    //Handshake command
    cmd = parse_number(cmd_string, 'H', -1);
    switch((int)(cmd)){
        case 0: // Handshake stuff to be implemented
            break;
        default: break;
    }

    //Request command
    cmd = parse_number(cmd_string, 'R', -1);
    switch((int)(cmd)){
        case 0: //Flag to write data
            write_data = 1;
            break;

        //If no matches, break
        default: break;
    }

    //Set value/mode commands
    cmd = parse_number(cmd_string, 'S', -1);
    switch(int(cmd)){
        case 0: //Set PID gains
            kp = (double)(parse_number(cmd_string, 'P', -1));
            ki = (double)(parse_number(cmd_string, 'I', -1));
            kd = (double)(parse_number(cmd_string, 'D', -1));
            break;

        case 1://Set Setpoint
            setpoint = (double)(parse_number(cmd_string, 'Z' , -1));
            break;

        case 2: //Set Lab type
            labType = (int)(parse_number(cmd_string, 'Y', -1));
            break;

        case 3: //Set Controller Mode (on/off)
            mode = (int)(parse_number(cmd_string, 'M', -1));
            break;

        case 4: //Set Sample Period
            sampleTime = parse_number(cmd_string, 'T', -1);
            break;

        case 5: //Set Output Limits
            lowerOutputLimit = (double)(parse_number(cmd_string, 'L', -1));
            upperOutputLimit = (double)(parse_number(cmd_string, 'U', -1));
            break;

        case 6: //Openloop control
            open_loop_voltage = (double)(parse_number(cmd_string,'O',0));
            break;

        case 7: //Feed Foward Voltage
            FF_A = (double)(parse_number(cmd_string,'A',0));
            FF_B = (double)(parse_number(cmd_string,'B',0));
            FF_C = (double)(parse_number(cmd_string,'C',0));
            break;
        
        case 8: //Open Loop Step Resonse Analysis
            open_loop_analysis_start = true;
            open_loop_analysis_time = (double)(parse_number(cmd_string, 'T', -1));
            break;

        case 9:
            calibration_start = true;
            break;
        
        case 10:
            anti_windup_activated = (int)(parse_number(cmd_string, 'W', -1));
            break;

        default: break;
    }
}

double SerialComms::parse_number(char* cmd_string, char key, int def){
    //Search cmd_string for key, return the number between key and delimiter
    // Serial.println(cmd_string);
    // Serial.println(key);

    int key_len=0; //Position of key in string
    int delim_len=0; //Position of next delimiter after key in string

    //Search string for first instance of key, increment key length each time key isn't found
    for(int i=0; i<100; i++) //TODO: Make this 100 value a HEADER_LENGTH #define
    {
        if(cmd_string[i] == '\0') { return def; } //If we can't find key, return default value
        if(cmd_string[i] == key){key_len = i; break;}
    }
    // Serial.print("key len: "); Serial.println(key_len);

    //Search string starting at character after key, looking for next delimiter the comma
    for(int i=key_len+1; i<100; i++){
        if(cmd_string[i] == ',' || cmd_string[i] == '\0')
        {
            break;
        }
        delim_len++;
    }
    // Serial.print("delim len: "); Serial.println(delim_len);

    //Create empty substring to use strncpy
    char substring[20] = {0};
    strncpy(substring, &cmd_string[key_len+1], delim_len);  //Copy subset of string to substring
    
    // Serial.print("test string: "); Serial.println(substring);
    return atof(substring); //return the substring in float format
}

void SerialComms::handle_command(){
// Arduino command handler
  if (Serial.available() != 0) {
    incoming_char = Serial.read();
    cmd[cmd_index] = incoming_char;
    if (incoming_char == '\0' || incoming_char == '%') {
      //      Serial.println("End of line, processing commands!");
      process_command(cmd);
      // Reset command buffer
      cmd_index = 0;
      memset(cmd, '\0', sizeof(cmd));
    }
    else {
      cmd_index ++;
    }
  }
}

void SerialComms::send_data(double enc_deg, double motor_speed, double motor_voltage) {
  // Check and if there is a request, send data
  if (write_data == 1) {
    Serial.print("T"); Serial.print(micros()); Serial.print(',');
    Serial.print('S'); Serial.print(setpoint); Serial.print(',');
    Serial.print('A');
    if (labType == 0) { // Angle
      Serial.print(enc_deg);
    }
    else if (labType == 1 || labType == 2) {
      Serial.print(motor_speed);
    }

    Serial.print(',');
    Serial.print('Q'); Serial.print(motor_voltage); Serial.print(',');
    Serial.println('\0');
    write_data = 0; // Reset write data flag
  }
}