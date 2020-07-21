long randNumber1;
long randNumber2;
float t;
float sine;

void setup() {
  Serial.begin(9600);
  t = 0;
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
}

void loop() {
  // print a random number from -10 to 10
  randNumber1 = random(-10,11);
  randNumber2 = random(-10, 11);
  sine = 10.0*sin(t);
  t += 0.1;
  Serial.print(randNumber1);
  Serial.print(",");
  Serial.println(sine);
  delay(50);
}
