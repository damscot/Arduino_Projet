/* 
 Controlling a servo position using a potentiometer (variable resistor) 
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val = 0;    // variable to read the value from the analog pin
int i = 5;
int val2 = 0;

void setup()
{
  Serial.begin (115200);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() 
{ 
  
  val += i; /* analogRead(potpin);*/            // reads the value of the potentiometer (value between 0 and 1023) 
  //Serial.println (val);
  if (val >= 1023)
    i = -5;
  if (val <= 0)
    i = 5;
  
  //val2 = map(val, 0, 1023, 40, 150);     // scale it to use it with the servo (value between 0 and 180) 
  //myservo.write(val2);                  // sets the servo position according to the scaled value 
  //Serial.println (val2);
  
  delay(10);                           // waits for the servo to get there 
  myservo.write(40);
  Serial.println("40");
  delay(2000);
  myservo.write(90);
  Serial.println("90");
  delay(2000);
    myservo.write(150);
  Serial.println("150");
  delay(2000);
} 

