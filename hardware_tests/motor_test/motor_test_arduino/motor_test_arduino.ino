/* This scripts works with arduino + motor board alone, without the python GUI
 *  Prints encoder measured speed in RPM to serial port 
 *  Spins motor (B port on arduino motor shield) at full power
 *  
 *  Harry Wei - 2020/7/25, adapted from https://github.com/jumejume1/Arduino/blob/master/ROTARY_ENCODER/ROTARY_ENCODER.ino
 */

#define ENC_A 2
#define ENC_B 3

#define PWM_B 11
#define BRK_B 8
#define DIR_B 13

float enc_count = 0;

void setup() {
  Serial.begin (19200);
  pinMode(ENC_A, INPUT_PULLUP); // internally pullup encoder input 
  pinMode(ENC_B, INPUT_PULLUP); // internally pullup encoder input 
  digitalWrite(DIR_B,LOW);  //Spin motor CW
  
  //Setting up interrupt
  attachInterrupt(digitalPinToInterrupt(ENC_A), Arise, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), Afall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), Brise, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), Bfall, FALLING);
}

int PWM = 0;
void loop() {
  if(PWM <= 255){
    analogWrite(PWM_B,PWM);
    double cur_time = micros();
    double time_stamp = micros();
    while(time_stamp-cur_time < 100000){
      Serial.print(time_stamp);
      Serial.print(",");
      Serial.print(enc_count);
      Serial.print(",");
      Serial.println(PWM);
      time_stamp = micros();
    }
    PWM ++;
  }
  digitalWrite(PWM_B,LOW);
}


void Arise() {
  if(digitalRead(ENC_B) == LOW){enc_count ++;}
  else{enc_count --;}
}

void Afall() {
  if(digitalRead(ENC_B) == HIGH){enc_count ++;}
  else{enc_count --;}
}

void Brise() {
  if(digitalRead(ENC_A) == HIGH){enc_count ++;}
  else{enc_count --;}
}

void Bfall() {  
  if(digitalRead(ENC_A) == LOW){enc_count ++;}
  else{enc_count --;}
}
