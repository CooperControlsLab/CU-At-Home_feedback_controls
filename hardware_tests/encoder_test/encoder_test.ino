/* This scripts works with arduino + motor board alone, without the python GUI
 *  Prints encoder measured speed in RPM to serial port 
 *  Spins motor (B port on arduino motor shield) at full power
 *  
 *  Harry Wei - 2020/7/25, adapted from https://github.com/jumejume1/Arduino/blob/master/ROTARY_ENCODER/ROTARY_ENCODER.ino
 */

int counter = 0;  // This variable will increase or decrease depending on the rotation of encoder
#define ENC_A 2
#define ENC_B 3

int count = 0;

void setup() {
  Serial.begin (19200);
  pinMode(ENC_A, INPUT_PULLUP); // internally pullup encoder input 
  pinMode(ENC_B, INPUT_PULLUP); // internally pullup encoder input 
  
  //Setting up interrupt
  attachInterrupt(digitalPinToInterrupt(ENC_A), Arise, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), Afall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), Brise, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), Bfall, FALLING);
}
   
void loop() {
  delay(100);
  Serial.println(count);
  delay(10);
}


void Arise() {
  if(digitalRead(ENC_B) == LOW){count ++;}
  else{count --;}
}

void Afall() {
  if(digitalRead(ENC_B) == HIGH){count ++;}
  else{count --;}
}

void Brise() {
  if(digitalRead(ENC_A) == HIGH){count ++;}
  else{count --;}
}

void Bfall() {  
  if(digitalRead(ENC_A) == LOW){count ++;}
  else{count --;}
}
