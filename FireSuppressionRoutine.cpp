#include <SoftwareSerial.h>
#include "enes100.h"
#include <Servo.h>

void suppressFire();
boolean detectFire();
const int DOWN = 180;
const int UP = 0;
int count = 0;
const int COUNTMAX = 24000;
const int COUNTMIN = 2000;
Servo lift;

void setup() 
{
  Serial.begin(9600);
  pinMode(A0,INPUT);//communication pin, input from main arduino
  pinMode(A1, OUTPUT);//communication pin, output to main arduino
  pinMode(7,OUTPUT);//fan pin
  lift.attach(9);//lift servo pin
  lift.write(90);//stops servo from rotating
  pinMode(A2, INPUT);//temp sensor pin
  analogWrite(A1,0);// tells main arduino that this arduino isn't doing anything
}

void loop() 
{  
  while(analogRead(A0) < 200){}//waits for signal from main arduino
  while(count < COUNTMAX && !detectFlame()) //raises lift and checks for flame
  {
    lift.write(UP);
    count++;
  }
  lift.write(90); 
  
  if(detectFlame())
  {
    suppressFlame();
    analogWrite(A1,255);
  }
  else
  {
    analogWrite(A1,100);
  }

  while(count > COUNTMIN)
  {
    lift.write(DOWN);
    count--;
  }
  lift.write(90);
  analogWrite(A1,0);//sends signal to main Arduino signaling the process is over
}

boolean detectFlame()
{
  if(analogRead(A2) < 100)
  {
    //Serial.println(analogRead(A2));
    return true;
  }
  else
  {
    //Serial.println(analogRead(A2));
    return false;
  }
}

void suppressFlame()
{
  int timeout = 0;
  while(detectFlame() && timeout <= 5)
  {
    digitalWrite(7,HIGH);//turns on fan
    delay(1000);
    timeout++;
  }
  digitalWrite(7,LOW);//turns off fan
}


