#include <g_code_interpreter.h>
#include <string.h>
#include <Arduino.h>

SerialComms::SerialComms(){
    setpoint = 0;
    labType = 0;
    mode = 1;
    lowerOutputLimit = -125;
    upperOutputLimit = 125;
    sampleTime = 0;
    kp = 0;
    ki = 0;
    kd = 0;
    write_data = 0;
    open_loop_PWM = 0;
}

void SerialComms::process_command(char* cmd_string){
    int pos;
    int cmd;

    //Handshake command
    cmd = parse_number(cmd_string, 'H', -1);
    switch(int(cmd)){
        case 0: // Handshake stuff to be implemented
            break;
        default: break;
    }

    //Request command
    cmd = parse_number(cmd_string, 'R', -1);
    switch(int(cmd)){
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
            kp = double(parse_number(cmd_string, 'P', -1));
            ki = double(parse_number(cmd_string, 'I', -1));
            kd = double(parse_number(cmd_string, 'D', -1));
            break;

        case 1://Set Setpoint
            setpoint = double(parse_number(cmd_string, 'Z' , -1));
            break;

        case 2: //Set Lab type
            labType = int(parse_number(cmd_string, 'Y', -1));
            break;

        case 3: //Set Controller Mode (on/off)
            mode = int(parse_number(cmd_string, 'M', -1));
            break;

        case 4: //Set Sample Time
            sampleTime = double(parse_number(cmd_string, 'T', -1));
            break;

        case 5: //Set Output Limits
            lowerOutputLimit = double(parse_number(cmd_string, 'L', -1));
            upperOutputLimit = double(parse_number(cmd_string, 'U', -1));
            break;

        case 6: //Openloop control
            open_loop_PWM = int(parse_number(cmd_string,'O',0));
            break;

        default: break;
    }
}

float SerialComms::parse_number(char* cmd_string, char key, int def){
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