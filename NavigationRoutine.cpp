#include <SoftwareSerial.h>
#include "enes100.h"

SoftwareSerial sSerial(4,5);
enes100::RfClient<SoftwareSerial> rf(&sSerial);
enes100::Marker marker;

void turn(double rad);
void turnLeft();
void turnRight();
void disableMotors();
void enableMotors();
void forward();
void changeSpeed(int x);
void gotoX(int x);
void gotoY(int y);
void surpressFire();
const double turnBuffer = .2;
const double xOffset = 0;
const double yOffset = 0;
const int markNum = 21;

void setup() 
{
  delay(100);
  sSerial.begin(9600);
  Serial.begin(9600);
  pinMode(7,INPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,INPUT);
  analogWrite(A0,0);
  delay(500);
  rf.sendMessage("Team Connected");
  gotoY(1.6);
  rf.sendMessage("gotoY #1 completed");
  turn(0);
  while(digitalRead(7) != 0 && marker. x <  1.09)
  {
    forward();
    rf.receiveMarker(&marker,markNum);
  }
  changeSpeed(0);
  if(digitalRead(7) == 0)
  {
    rf.sendMessage("THE WALL!");
    delay(500);
    gotoY(0.19);
    gotoX(1.09);
  }
  gotoY(1.13);
  gotoX(2.6);
  turn(PI);
  while(marker.y < 1.3 && !digitalRead(7) == 0)
  {
    forward();
    rf.receiveMarker(&marker,markNum);
  }
  changeSpeed(0);
  surpressFire();
  
  
}

void loop(){}

void turnLeft()
{
  changeSpeed(100);
  digitalWrite(12,HIGH);
  digitalWrite(13,LOW); 
}

void turnRight()
{
  changeSpeed(100);
  digitalWrite(12,LOW);
  digitalWrite(13,HIGH);
}

void forward()
{
  changeSpeed(100);
  digitalWrite(12,LOW);
  digitalWrite(13,LOW);
}

void turn(double rad)
{
  delay(100);
  rf.receiveMarker(&marker,markNum);
  int currAngle = marker.theta;
  if(rad - currAngle > 0)
  {
    while(!sameAngle(currAngle,rad))
    {
      turnLeft();
      rf.receiveMarker(&marker,markNum);
      currAngle = marker.theta;
    }
  }
  else
  {
    while(!sameAngle(currAngle,rad))
    {
      turnRight();
      rf.receiveMarker(&marker,markNum);
      currAngle = marker.theta;
    }
  }
  //rf.sendMessage("turn completed");
  changeSpeed(0);
}

void gotoX(double x)
{
  delay(100);
  rf.receiveMarker(&marker,markNum);
  if(marker.x < x)
  {
    turn(0);
    forward();
    while(marker.x + xOffset < x)
    {
      rf.receiveMarker(&marker,markNum);
    }
    changeSpeed(0);
  }
  else
  {
    turn(PI);
    forward();
    while(marker.x + xOffset > x)
    {
      rf.receiveMarker(&marker,markNum);
    }
    changeSpeed(0);
  }
}

void gotoY(double y)
{
  delay(100);
  rf.receiveMarker(&marker,markNum);
  if(marker.y < y)
  {
    turn(PI/2);
    rf.sendMessage("turn in gotoY completed");
    forward();
    while(marker.y + yOffset < y)
    {
      rf.receiveMarker(&marker,markNum);
    }
    changeSpeed(0);
  }
  else
  {
    turn(-PI/2);
    forward();
    while(marker.y + yOffset > y)
    {
      rf.receiveMarker(&marker,markNum);
    }
    changeSpeed(0);
  }
}

void changeSpeed(int newSpeed)
{
  analogWrite(3,newSpeed);
  analogWrite(11,newSpeed);
}

void enableMotors()
{
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
}

void disableMotors()
{
  digitalWrite(9,HIGH);
  digitalWrite(8,HIGH);  
}

boolean sameAngle(double currAngle, double newAngle)
{
    if (currAngle > newAngle - turnBuffer && currAngle < newAngle + turnBuffer)
    {
      //rf.sendMessage("sameAngle returned true");
      //Serial.println("sameAngle returned true");
      return true;
    }
    else
    {
      return false;
    }
}

void surpressFire()
{
  analogWrite(A0,255);
  while(map(analogRead(A1), 0, 1023, 0, 255) < 50)
  {
    if(map(analogRead(A1), 0, 1023, 0, 255) > 200)
    {
      rf.sendMessage("Flame Detected and (hopefully) extinguished!");
      analogWrite(A0,0);
      break;
    }
    if(map(analogRead(A1), 0, 1023, 0, 255) > 50)
    {
      rf.sendMessage("No Flame Detected!");
      analogWrite(A0,0);
      break;
    }
  }
  while(map(analogRead(A1), 0, 1023, 0, 255) > 50){} //waits for lift to lower
}

void debug()
{
      rf.sendMessage("theta: ");
      rf.sendMessage(marker.theta);
      rf.sendMessage(" x: ");
      rf.sendMessage(marker.x);
      rf.sendMessage(" y: ");
      rf.sendMessage(marker.y);
      rf.sendMessage(" ");
      rf.sendMessage("\n\n");
}

