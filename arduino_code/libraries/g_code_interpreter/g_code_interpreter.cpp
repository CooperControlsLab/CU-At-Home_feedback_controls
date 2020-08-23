#include <g_code_interpreter.h>
#include <string.h>
#include <Arduino.h>

void SerialComms::process_command(char* cmd_string){
    int pos;
    int cmd;
    int write_data;

    //Handshake command
    cmd = parse_number(cmd_string, 'H', -1);
    switch(int(cmd)){
        case 0: write_data = 1;
        default: break;
    }

    //Request command
    cmd = parse_number(cmd_string, 'R', -1);
    switch(int(cmd)){
        case 0: // Serial write the data packet back to GUI

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

        case 1://Set Setpoint
            setpoint = parse_number(cmd_string, 'Z' , -1);

        case 2: //Set Lab type
            labType = parse_number(cmd_string, 'Y', -1);

        case 3: //Set Controller Mode (on/off)
            mode = parse_number(cmd_string, 'M', -1);

        case 4: //Set Sample Time
            sampleTime = parse_number(cmd_string, 'T', -1);

        case 5: //Set Output Limits
            lowerOutputLimit = parse_number(cmd_string, 'L', -1);
            upperOutputLimit = parse_number(cmd_string, 'U', -1);

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
    
    Serial.print("test string: "); Serial.println(substring);
    return atof(substring); //return the substring in float format
}