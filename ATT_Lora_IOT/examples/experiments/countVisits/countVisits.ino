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
 * Explanation:
 * 
 * Each time the door opens a counter is incremented locally on your LoRa device.
 * Every 30 seconds, if the count has changed, it will be sent to your AllthingsTalk account.
 * As soon as a count of 20 is reached, a notification is sent out to remind you that cleaning is in order.
 * A pushbutton on the device allows you to reset the count when cleaning is done.
 * This can also be seen as validation that the cleaning crew has actually visited the facility.
 *
 **/

#include <Wire.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

int pushButton = 20;          
int doorSensor = 4;
MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);

#define SEND_MAX_EVERY 30000                                // the mimimum time between 2 consecutive updates of visit counts that are sent to the cloud (can be longer, if the value hasn't changed)

bool prevButtonState;
bool prevDoorSensor;
short visitCount = 0;                                       // keeps track of the nr of visitors         
short prevVisitCountSent = 0;                               // we only send visit count max every 30 seconds, but only if the value has changed, so keep track of the value that was last sent to the cloud.
unsigned long lastSentAt = 0;                               // the time when the last visitcount was sent to the cloud.
bool isConnected = false;                                   // keeps track of the connection state.

void setup() 
{
  pinMode(pushButton, INPUT);                               // initialize the digital pin as an input.          
  pinMode(doorSensor, INPUT);
  while((!Serial) && (millis()) < 2000){}					//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                // init the baud rate of the serial connection so that it's ok for the modem

  Serial.println("-- Count visits LoRa experiment --");
  Serial.print("Sending data at most every ");Serial.print(SEND_MAX_EVERY);Serial.println(" milliseconds");
  
  prevButtonState = digitalRead(pushButton);                // set the initial state
  prevDoorSensor = digitalRead(doorSensor);                 // set the initial state
  
}

void tryConnect()
{
  isConnected = Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  if(isConnected == true)
  {
     Serial.println("Ready to send data");
     sendVisitCount();                                  	 // always send the value at initial connection, keep the platform in sync with the latest change on the device.
  } 
  else
     Serial.println("Connection will by retried later");  
}


void loop() 
{
  if(isConnected == false)                                  // retry connecting if needed while already counting visits locally
    tryConnect();
  processButton();
  processDoorSensor();
  delay(100);
  if(isConnected && prevVisitCountSent != visitCount && lastSentAt + SEND_MAX_EVERY <= millis())	// only send a message when something has changed and SEND_MAX_EVERY has been exceeded
    sendVisitCount();
}

void sendVisitCount()
{
  Serial.print("Send visit count: ");Serial.println(visitCount);
  Device.Send(visitCount, INTEGER_SENSOR);                 // always send visit count to keep the cloud in sync with the device
  prevVisitCountSent = visitCount;
  lastSentAt = millis();
}

// check the state of the door sensor
void processDoorSensor()
{
  bool sensorRead = digitalRead(doorSensor);                         
  if(prevDoorSensor != sensorRead)
  {
    prevDoorSensor = sensorRead;
    if(sensorRead == true)                                // door was closed, so increment the counter 
    {
        Serial.println("Door closed");
        visitCount++;                                    // the door was opened and closed again, so increment the counter
		Serial.print("VisitCount: ");Serial.println(visitCount);
    }
    else
        Serial.println("Door open");
  }
}

void processButton()
{
  bool sensorRead = digitalRead(pushButton);            // check the state of the button
  if (prevButtonState != sensorRead)                    // verify if value has changed
  {
     prevButtonState = sensorRead;
     if(sensorRead == true)                                         
     {
        Serial.println("Button pressed, counter reset");
        visitCount = 0;
     }
     else
        Serial.println("Button released");
  }
}

void SendValue(bool val)
{
  Serial.println(val);
  
}


void serialEvent1()
{
  Device.Process();                                     // for future extensions -> actuators
}


