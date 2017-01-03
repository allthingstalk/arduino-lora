/*
   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/  

/****
 *  AllThingsTalk Developer Cloud IoT experiment for LoRa
 *  Version 1.0 dd 09/11/2015
 *  Original author: Jan Bogaerts 2015
 *
 *  This sketch is part of the AllThingsTalk LoRa rapid development kit
 *  -> http://www.allthingstalk.com/lora-rapid-development-kit
 *
 *  This example sketch is based on the Proxilmus IoT network in Belgium
 *  The sketch and libs included support the
 *  - MicroChip RN2483 LoRa module
 *  - Embit LoRa modem EMB-LR1272
 *  
 *  For more information, please check our documentation
 *  -> http://allthingstalk.com/docs/tutorials/lora/setup
 *  
 **/

#include <Wire.h>
#include <MMA7660.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600


MMA7660 accelemeter;                                  // connect to the I2C port (SCL/SDA)
MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);

void setup() 
{
  accelemeter.init();
  while((!Serial) && (millis()) < 2000){}				//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());			// init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float accx, accy, accz;
int8_t x,y,z; 

void loop() 
{
  accelemeter.getXYZ(&x, &y, &z);
  Serial.println("Values");
  Serial.print("  x: ");
  Serial.println(x);
  Serial.print("  y: ");
  Serial.println(y);
  Serial.print("  z: ");
  Serial.println(z);
  
  accelemeter.getAcceleration(&accx, &accy, &accz);
  Serial.println("Accleration");
  Serial.print("  x: ");
	Serial.print(accx);
  Serial.println(" g");
  Serial.print("  y: ");
	Serial.print(accy);
	Serial.println(" g");
	Serial.print("  z: ");
	Serial.print(accz);
	Serial.println(" g");
	Serial.println();

  SendValue();
  
  delay(1000);
}

void SendValue()
{
  Device.Queue(accx);
  Device.Queue(accy);
  Device.Queue(accz);
  Device.Send(ACCELEROMETER);
}


void serialEvent1()
{
  Device.Process();														// for future use of actuators
}


