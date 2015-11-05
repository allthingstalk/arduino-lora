/*
AllThingsTalk - SmartLiving.io LoRa Arduino demos
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include "ATT_LoRa_IOT.h"
#include "keys.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


#define AnalogSensor A4


MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  pinMode(AnalogSensor, INPUT);					            // initialize the digital pin as an input.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float value = 1.1;

void loop() 
{  
  analogRead(AnalogSensor);
  delay(10);
  unsigned int val = analogRead(AnalogSensor);
  Serial.println(val);
  //SendValue();
  delay(200);
}

void SendValue()
{
  Serial.println(value);
  Device.Send(value, LOUDNESS_SENSOR);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


