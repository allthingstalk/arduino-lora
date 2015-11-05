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

bool value = true;

void loop() 
{
  SendValue(value);
  value = !value;
  delay(300);
}

void SendValue(bool val)
{
  Serial.println(val);
  Device.Send(val, PIR_SENSOR);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


