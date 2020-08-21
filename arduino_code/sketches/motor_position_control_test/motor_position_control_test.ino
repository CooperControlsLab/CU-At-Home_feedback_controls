#include <PID_v1.h>
#define ENC_CHAN_A 2
#define ENC_CHAN_B 4
#define DIR_A 12
#define DIR_B 13
#define PWM_A 3
#define PWM_B 11
#define CURRENT_SENSE_A 0
const double PPR = 9.68*12.0; //pulses per revolution of the encoder
#define PWM_MAX 200
const double degrees_per_count = 360.0 / PPR;
double angle; //Variable to store angle converted from encoder count
volatile int enc_count = 0; //Encoder counting variable
double current_a;
const double current_factor = 2/3.3;// 3.3V per 2A which is 0.909

//Initialize PID, declare PID variables
double input;
double output;
double setpoint = 100;
double kp = 10;
double ki;
double kd=1;
PID control_position(&input, &output, &setpoint, kp, ki, kd, REVERSE);


void setup() {
  //begin serial comms
  Serial.begin(9600);

  //Set pin modes
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(ENC_CHAN_A, INPUT);

  //Set PID params
  control_position.SetSampleTime(25);
  control_position.SetMode(1);
  control_position.SetOutputLimits(-100, 100);

  //Set interrupt connection
  attachInterrupt(digitalPinToInterrupt(ENC_CHAN_A), do_count, RISING);

}

void loop() {
  //Print for debugging
  Serial.print("angle: "); Serial.print(angle); Serial.print(" current: "); Serial.println(current_a);

  //Get current through motor
  current_a = map(analogRead(CURRENT_SENSE_A), 0, 1024, 0, 5000)*current_factor;
  
  //Convert encoder count to degrees
  angle = (double)enc_count * (double)degrees_per_count;

  //Update input parameter for PID controller
  input = angle;

  //Compute PID
  control_position.Compute();

  //Set motor control
  set_motor(output);
}

void do_count()
{
  if (digitalRead(ENC_CHAN_B) == 1) {
    enc_count++;; //If chan B is HIGH, increment
  }
  else {
    enc_count--; //Else decrement
  }
}

void set_motor(double power)
{
  if (power < 0) {
    digitalWrite(DIR_A, LOW);
  }
  else {
    digitalWrite(DIR_A, HIGH);
  }
  analogWrite(PWM_A, abs(power));
}
