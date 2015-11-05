/*
AllThingsTalk - SmartLiving.io LoRa Arduino demos
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float value = 1.1;

void loop() 
{
  SendValue();
  value = value + 1.01;
  delay(300);
}

void SendValue()
{
  Serial.println(value);
  Device.Send(value, HUMIDITY_SENSOR);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


