#include <HardwareSerial.h>

#define forceSensor 35
#define heartRead 34
#define heartLOplus 19
#define heartLOminus 18
#define RXD2 16
#define TXD2 17

HardwareSerial Sender(2);

void setup()
{
  // put your setup code here, to run once:
  pinMode(forceSensor, INPUT);
  pinMode(heartRead, INPUT);
  pinMode(heartLOminus, INPUT);
  pinMode(heartLOplus, INPUT);

  Sender.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop()
{
  // put your main code here, to run repeatedly:
  int heartData = analogRead(heartRead);
  int forceData = analogRead(forceSensor);
  Sender.print("LO+: ");
  Sender.print(digitalRead(heartLOplus));

  Sender.print("\tLO-: ");
  Sender.print(digitalRead(heartLOminus));

  Sender.print("\tHeart data: ");
  Sender.print(heartData);

  Sender.print("\tForce sensor: ");
  Sender.println(forceData);

  

  delay(10);
}
