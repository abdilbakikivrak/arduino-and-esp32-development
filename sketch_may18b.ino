/*  Motor PID Controller V1
     Aim: Recieve a position from button presses incrementing 10 degrees. The motor then turns to this angle using
     the position sensor and a pid controller to position itself.

     Hardware: Tested with Arduino Uno, 6v DC electric motor, L298N H_Bridge controller, AMS-AS5048b Magnetic Rotary Encoder.

*/


//Required Headders
//#include <PID_v1.h>
#include <ams_as5048b.h>

//Motor Pins
#define EN    9    //Motor Enable(Speed) Pin
#define CW   10
#define CCW  11

//Button Pins
#define Up_Butt 2
#define Dn_Butt 3

int delta = 1;

//Set up AS504b (required constants)
#define U_RAW 1
#define U_TRN 2
#define U_DEG 3
#define U_RAD 4
#define U_GRAD 5
#define U_MOA 6
#define U_SOA 7
#define U_MILNATO 8
#define U_MILSE 9
#define U_MILRU 10

AMS_AS5048B sensor1(0x40); //0x40 is this partiuclar sensors i2c address.

//Set up PID
double Setpoint = 0, Input, Output; //Setpoint = Target, Input = Measured Value, Output = Motor speed??
double Kp = 5, Ki = 1, Kd = 0.01;    //PID Constants (Just guesses at this point)


PID pid1(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  Serial.begin(9600);

  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(Up_Butt, INPUT);
  pinMode(Dn_Butt, INPUT);

  sensor1.begin();
  sensor1.setZeroReg();   //Current Position is set as 0 angle.
  sensor1.setClockWise(true);

  TCCR1B = TCCR1B & 0b11111000 | 1;  // set 31KHz PWM to prevent motor noise
  pid1.SetMode(AUTOMATIC);   //set PID in Auto mode
  pid1.SetSampleTime(1);  // refresh rate of PID controller
  pid1.SetOutputLimits(-255, 255); // this is the MAX PWM motor value

}

void loop() {
  //Start by getting target position (setpoint)
  if (digitalRead(Up_Butt) == HIGH) Setpoint = Setpoint + delta;
  if (digitalRead(Dn_Butt) == HIGH) Setpoint = Setpoint - delta;

  //Limit value between 0-360
  if (Setpoint > 360) Setpoint = 360;
  if (Setpoint < 0) Setpoint = 0;

  //Get current angle (input)
  Input = sensor1.angleR(U_DEG, true);

  //Run Pid
  pid1.Compute();

  //Send To Motor
  if (Output > 0) {
    clockwise();
    analogWrite(EN, abs(Output));
  }
  else if (Output < 0) {
    anticlockwise();
    analogWrite(EN, abs(Output));
  }
  else  {
    motor_off();
  }

  //Print Numbers for tracking
  Serial.print("Target Angle:  "); Serial.print(Setpoint);
  Serial.print("   Measured Angle:   "); Serial.print(Input);
  Serial.print("   Calculated Motor Speed:   "); Serial.println(Output);
}


/////////Motor Control////////////
void clockwise() {
  digitalWrite(CW, HIGH);
  digitalWrite(CCW, LOW);
}

void anticlockwise() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, HIGH);
}

void motor_off() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, LOW);
}
