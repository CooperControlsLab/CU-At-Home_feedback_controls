#include <g_code_interpreter.h>
#include <string.h>

void SerialComms::processCommand(char* cmd_string)
{
    int pos;
    int cmd;

    //Request command
    // cmd = parseNumber(cmd_string, 'R', -1);
    switch(cmd){
        case 0: // Serial write the data packet back to GUI

        //If no matches, break
        default: break;
    }

    //Set value/mode commands
    cmd = parseNumber(cmd_string, 'S', -1);
    switch(int(cmd)){
        case 0: 
            double kp = parseNumber(cmd_string, 'P', -1);
            double ki = parseNumber(cmd_string, 'I', -1);
            double kd = parseNumber(cmd_string, 'D', -1);

        case 1://Set Setpoint

        case 2: //Set Lab type

        case 3: //Set Controller Mode (on/off)

        case 4: //Set Sample Time

        case 5: //Set Output Limits


        default: break;
    }
}

float SerialComms::parseNumber(char* cmd_string, char key, int def)
{
    //Search cmd_string for key, return the number between key and delimiter
    // Serial.println(cmd_string);
    // Serial.println(key);

    int key_len=0; //Position of key in string
    int delim_len=0; //Position of next delimiter after key in string

    //Search string for first instance of key, increment key length each time key isn't found
    for(int i=0; i<100; i++)
    {
        if(cmd_string[i] == key){key_len = i; break;}
    }
    // Serial.print("key len: "); Serial.println(key_len);

    //Search string starting at character after key, looking for next delimiter the comma
    for(int i=key_len+1; i<100; i++)
    {

        // Serial.print("i: "); Serial.print(i); Serial.print(" | "); Serial.println(first[i]);
        if(cmd_string[i] == ',' || cmd_string[i] == '\0')
        {
            delim_len = i;
            break;
        }

    }
    // Serial.print("delim len: "); Serial.println(delim_len);

    //Create empty substring to use strncpy
    char substring[20];
    strncpy(substring, &cmd_string[key_len+1], delim_len);  //Copy subset of string to substring
    
    // Serial.print("test string: "); Serial.println(substring);
    return atof(substring); //return the substring in float format
}