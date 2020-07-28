#include <PID_v1.h>
#define ENC_CHAN_A 18
#define ENC_CHAN_B 19
#define ENC_PROP_A 20
#define ENC_PROP_B 21
#define DIR_A 12
#define DIR_B 13
#define PWM_A 3
#define PWM_B 11
#define PPR 600 //pulses per revolution of the encoder

//Global Variables
//Encoder Counting Variables, Encoder Parameters
volatile int enc_count=0; //Encoder counting variable
volatile int prop_count_a=0; //Prop A Counter
volatile int prop_count_b=0; //Prop B Counter

//PID controller variables
double input, output;
double setpoint=25;
double kp = 1.5;
double ki = 0.1;
double kd = 0.5;
int base_pwm = 50; //Default power for both propellers
PID balance(&input, &output, &setpoint, kp, ki, kd, DIRECT);

void setup() {
  //Set digial pin modes
  pinMode(ENC_CHAN_A, INPUT_PULLUP);
  pinMode(ENC_CHAN_B, INPUT_PULLUP);
  pinMode(ENC_PROP_A, INPUT);
  pinMode(ENC_PROP_B, INPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  //Set interrupt connection
  attachInterrupt(digitalPinToInterrupt(ENC_CHAN_A), do_count, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_PROP_A), do_prop_count_a, RISING);

  //PID controller settings
  balance.SetMode(1);
  balance.SetOutputLimits(-50, 50);
  balance.SetSampleTime(25);  //Setting sample time to 25ms here
  
  //Start serial for debugging
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("ENC COUNT: "); Serial.print(enc_count); 
  Serial.print(" PROP COUNT: "); Serial.println(prop_count_a);
  //Update input value from encoder
  input = enc_count;
  balance.Compute();
  set_prop_pwm(base_pwm + output, base_pwm - output);
}


//Encoder CHANNEL A has set off interrupt, check CHANNEL B to check direction
//Increment/Decrement counter variable accordingly
void do_count()
{
  if(digitalRead(ENC_CHAN_B) == 1){enc_count++;;} //If chan B is HIGH, increment
  else {enc_count--;}//Else decrement
}

void do_prop_count_a()
{
  prop_count_a++;
}

void do_prop_count_b()
{
  prop_count_b++;
}

void set_prop_pwm(int pwma, int pwmb)
{
  digitalWrite(DIR_A, LOW);
  analogWrite(PWM_A, pwma);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_B, pwmb);
}
