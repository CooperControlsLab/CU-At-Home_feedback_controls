int LED1 = 10;
int LED2 = 11;
int value1;
int value2;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  while (Serial.available()>0)
    {
    if (Serial.available()>0){
      if (Serial.peek() == 'a'){
        Serial.read();
        value1 = Serial.parseInt();
        analogWrite(LED1,value1);
      }
      else if (Serial.peek() == 'b'){
        Serial.read();
        value2 = Serial.parseInt();
        analogWrite(LED2,value2);
      }
    }
  
    
    }
}
