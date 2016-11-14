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
 *  AllThingsTalk Developer Cloud IoT demo for LoRa
 *  
 *  This sketch is part of the AllThingsTalk LoRa rapid development kit
 *  -> http://www.allthingstalk.com/lora-rapid-development-kit
 *
 *  This demo sketch is based on the Proximus IoT network in Belgium
 *  The sketch and libs included support the
 *  - MicroChip RN2483 LoRa module
 *  - Embit LoRa modem EMB-LR1272
 *  
 *  For more information, please check our documentation
 *  -> http://allthingstalk.com/docs/tutorials/lora/setup
 *
 * Explanation:
 * 
 * We want to check the environment inside our Banana container as well as keeping track
 * of the container door.
 * 
 * The status of the door is checked continuously and an alert is sent right away when the
 * container door is opened. An environmental value is sent every 60 seconds, rotating
 * between temperature, humidity and pressure.
 * 
 **/

#include <Wire.h>
#include <Sodaq_TPH.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

int doorSensor = 4;

MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);

bool currentDoorValue;
bool alertSent = false;
bool sentOnce = false;

float temp;
float hum;
float pres;
int TPHState = 0;   // Keeps track of which TPH value to send

long lastSent = 0;

bool isConnected = false;                                   // Keeps track of the connection state

void setup() 
{
  pinMode(doorSensor, INPUT);
  while((!Serial) && (millis()) < 2000){}					//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  
  Serial1.begin(Modem.getDefaultBaudRate());                // Init the baud rate of the serial connection so that it's ok for the modem

  Serial.println("   _");
  Serial.println("  | |__   __ _ _ __   __ _ _ __   __ _ ___ ");
  Serial.println("  | '_ \\ / _` | '_ \\ / _` | '_ \\ / _` / __|");
  Serial.println("  | |_) | (_| | | | | (_| | | | | (_| \\__ \\");
  Serial.println("  |_.__/ \\__,_|_| |_|\\__,_|_| |_|\\__,_|___/");
  Serial.println("                          Banana Food Inc.");
  Serial.println("");
  
  currentDoorValue = digitalRead(doorSensor);                 // Set the initial state
  
  if(isConnected == false)                                    // Retry connecting LoRa communication
    tryConnect();
  delay(5000);
  Serial.println("Initialize doorstatus");
  sendDoorSensor(false);    // init doorstatus
  Serial.println("Waiting for first payload to be sent in approx 1 minute"); 
}

void loop() 
{
  if(isConnected == false)                                  // Retry connecting LoRa communication
    tryConnect();

 
  readDoorSensor();    										// Check status of container door

  if(millis() - lastSent > 60000)     						// Send a TPH value every 60 second
  {
    lastSent = millis();
    readTPHSensor();           								// Check container environment values
    sendTPHSensor();

    TPHState >= 2 ? TPHState = 0 : TPHState++;             	// Loop through TPH sensors 0 = temperature, 1 = humidity, 2 = pressure
    Serial.println("Waiting for next payload to be sent in approx 1 minute"); 
  }

  if(!currentDoorValue && !sentOnce)
  {
    sendDoorSensor(true);    								// Door is opened!
    sentOnce = true;
  }
  
  delay(100);
}

void tryConnect()
{
  isConnected = Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  if(isConnected == true)
    Serial.println("Ready to send data");
  else
    Serial.println("Connection will by retried later");  
}

/************************************************************************************
 * Read sensor values
 */

void readDoorSensor()
{
  bool sensorRead = digitalRead(doorSensor);
  
  if(currentDoorValue != sensorRead)                 // Container door value changed
  {
    currentDoorValue = sensorRead;
    if(sensorRead == false)
    {
      Serial.println("Container open");
      //alertSent = false;
    }
    else
      Serial.println("Container closed");
  }
}

void readTPHSensor()
{
  if(TPHState == 0)
  {
    temp = tph.readTemperature();
    Serial.print("  Temperature: ");
    Serial.print(String(temp, 2));
    Serial.println(" deg C");
  }
  else if(TPHState == 1)
  {
    hum = tph.readHumidity();
    Serial.print("  Humidity   : ");
    Serial.print(String(hum, 2));
    Serial.println(" %");
  }
  else if(TPHState == 2)
  {
    pres = tph.readPressure()/100.0;
    Serial.print("  Pressure   : ");
    Serial.print(String(pres, 2));
    Serial.println(" hPa");
  }
  else
  {
    Serial.println("Unknown TPH state");
  }
}

/************************************************************************************
 * Send sensor values
 */

void sendDoorSensor(bool val)
{
  if(val) Serial.println("** Alert: Container door is opened!");
  else Serial.println("** Container door is closed");
  Device.Send(val, DOOR_SENSOR); 
}

void sendTPHSensor()
{
  if(TPHState == 0)
  {
    Serial.println("Sending TPH Temperature value");
    Device.Send(temp, TEMPERATURE_SENSOR);
  }
  else if(TPHState == 1)
  {
    Serial.println("Sending TPH Humidity value");
    Device.Send(hum, HUMIDITY_SENSOR);
  }
  else if(TPHState == 2)
  {
    Serial.println("Sending TPH Pressure value");
    Device.Send(pres, PRESSURE_SENSOR);
  }
}

/************************************************************************************
 * Callback for future actuators
 */
 
void serialEvent1()
{
  Device.Process();
}
