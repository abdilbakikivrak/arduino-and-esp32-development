#include <Wire.h>
#include <Servo.h>
int MPU=0x68;
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
Servo servomotor1;
Servo servomotor2;

void setup() {
  // put your setup code here, to run once:
Wire.begin();
Wire.beginTransmission (MPU);
Wire.write (0x6B);
Wire.write (0);
Wire.endTransmission (true) ;
servomotor1.attach (9);
servomotor1.attach (10);
}

void loop() {
  // put your main code here, to run repeatedly:

Wire.beginTransmission (MPU);
Wire.write (0x3B);
Wire.endTransmission (false) ;
Wire.requestFrom (MPU, 14, true);

AcX=Wire.read ()<<8|Wire.read();
AcY=Wire.read ()<<8|Wire.read();
AcZ=Wire.read ()<<8|Wire.read();
GyX=Wire.read ()<<8|Wire.read();
GyY=Wire.read ()<<8|Wire.read();
GyY=Wire.read ()<<8|Wire.read();

int durum_1 = map (AcY,0000, 13000,55,99);
int durum_2 = map (AcZ, -4000, 18000, 95,169);

servomotor1.write (durum_1 );
servomotor2.write (durum_2 );
delay (500);

}
