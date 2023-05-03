#include <Servo.h>

      Servo motor;
   int pos =0;

   void stup()
{
  motor.attach(9);
}
   void loop()
{ 
   for ( pos=0; pos <= 180; pos +=1)
{
   motor.write(pos);
 delay(20);

}
 for ( pos=180; pos >= 0; pos -=1)
{
  motor.write(pos);
  delay(20);
}
}
