/* This scripts works with arduino + motor board alone, without the python GUI
 *  Prints encoder measured speed in RPM to serial port 
 *  Spins motor (B port on arduino motor shield) at full power
 *  
 *  Harry Wei - 2020/7/25, adapted from https://github.com/jumejume1/Arduino/blob/master/ROTARY_ENCODER/ROTARY_ENCODER.ino
 */

int temp= 0; 
int counter = 0;  // This variable will increase or decrease depending on the rotation of encoder
float deg = 0;    // Angle rotated
float last_deg = 0; // Angle of last cycle
float cur_time = 0;
float last_time = 0;
float spd = 0;


void setup() {
  Serial.begin (9600);
  pinMode(2, INPUT_PULLUP); // internally pullup input pin 2 
  pinMode(3, INPUT_PULLUP); // internally pullup input pin 3
  
  //Setting up interrupt
  //Rising pulse from encoder port A activats ai0(). 
  attachInterrupt(0, ai0, RISING);
  //Rising pulse from encoden port B activats ai1().
  attachInterrupt(1, ai1, RISING);
  
  digitalWrite(11,HIGH);  // Motor full power spin
}
   
void loop() {
  // Send the value of counter
  if( counter != temp ){
  deg = 0.3*counter;
  // Serial.println (deg);
  temp = counter;
  }
  cur_time = millis();
  spd = (deg-last_deg)/(cur_time-last_time);  // Speed in deg/ms
  last_deg = deg;
  last_time = cur_time;
  spd = spd*1000/6;  // Speed in RPM
  Serial.println(spd);
}
   
void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(3)==LOW) {
  counter++;
  }else{
  counter--;
  }
}
   
void ai1() {
  // ai0 is activated if DigitalPin nr 3 is going from LOW to HIGH
  // Check with pin 2 to determine the direction
  if(digitalRead(2)==LOW) {
  counter--;
  }else{
  counter++;
  }
}
