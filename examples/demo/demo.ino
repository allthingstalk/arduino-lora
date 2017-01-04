/*
   Copyright 2015-2017 AllThingsTalk

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
 *  version 1.0 dd 09/11/2015
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
 */
//#include <Wire.h>
#include <ATT_IOT_LoRaWAN.h>
#include <MicrochipLoRaModem.h>
#include <Container.h>
#include "keys.h"

#define SERIAL_BAUD 57600

int DigitalSensor = 20;                              // pin this sensor is connected to
MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);
Container payload(Device);

bool sensorVal = false;
bool prevButtonState = false;

void setup() 
{
  pinMode(DigitalSensor, INPUT);               			// initialize the digital pin as an input
  while((!Serial) && (millis()) < 2000){}				//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);                   			// set baud rate of the default serial debug connection
  Serial1.begin(Modem.getDefaultBaudRate());   			// set baud rate of the serial connection between Mbili and LoRa modem
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))
	Serial.println("retrying...");						// initialize connection with the AllThingsTalk Developer Cloud
  Serial.println("Ready to send data");

  sensorVal = digitalRead(DigitalSensor);
  SendValue(sensorVal);                        			// send initial state
}

void loop() 
{
	bool sensorRead = digitalRead(DigitalSensor);      	// read status Digital Sensor
	if (sensorRead == 1 && prevButtonState == false)      // verify if value has changed
	{
		prevButtonState = true;
		if(SendValue(!sensorVal) == true)
			sensorVal = !sensorVal;
	}
	else if(sensorRead == 0)
		prevButtonState = false;
	Device.ProcessQueuePopFailed();
}

bool SendValue(bool val)
{
  Serial.print("Data: ");Serial.println(val);
  bool res = payload.Send(val, BINARY_SENSOR);
  if(res == false)
    Serial.println("maybe the last couple of packages were sent too quickly after each other? (min of 15 seconds recommended)");
  return res;
}

