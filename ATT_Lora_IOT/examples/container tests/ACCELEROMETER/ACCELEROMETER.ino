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
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY)) delay(200);
  Serial.println("Ready to send data");
}

float x = 1.1;
float y = 2.2;
float z = 3.3;

void loop() 
{
  SendValue();
  x = x + 10.01;
  y = y + 10.01;
  z = z + 10.01;
  delay(7000);
}

void SendValue()
{
  Device.Queue(x);
  Device.Queue(y);
  Device.Queue(z);
  Device.Send(ACCELEROMETER);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


