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
 * We will measure our environment using 6 sensors. Approximately, every 2 minutes, all values 
 * will be read and sent to the AllthingsTalk Developer Cloud.
 * 
 **/

#include <Wire.h>
#include <Sodaq_TPH.h>
#include "AirQuality2.h"
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>


#define SERIAL_BAUD 57600
#define AirQualityPin A0
#define LightSensorPin A2
#define SoundSensorPin A4

#define SEND_EVERY 20000


MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);
AirQuality2 airqualitysensor;

float soundValue;
float lightValue;
float temp;
float hum;
float pres;
short airValue;

void setup() 
{
  pinMode(GROVEPWR, OUTPUT);                                    // turn on the power for the secondary row of grove connectors.
  digitalWrite(GROVEPWR, HIGH);
  while((!Serial) && (millis()) < 2000){}						//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    // init the baud rate of the serial connection so that it's ok for the modem
  
  Serial.println("-- Environmental Sensing LoRa experiment --");
  Serial.print("Sending data each ");Serial.print(SEND_EVERY);Serial.println(" milliseconds");

  Serial.println("Initializing modem");
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))            // there is no point to continue if we can't connect to the cloud: this device's main purpose is to send data to the cloud.
  {
    Serial.println("Retrying...");
    delay(200);
  }
  Device.SetMinTimeBetweenSend(15000);                          // wait between sending 2 messages, to make certain that the base station doesn't punish us for sending too much data too quickly, default = 0.
  initSensors();
}

void loop() 
{
  ReadSensors();
  DisplaySensorValues();
  SendSensorValues();
  Serial.print("Delay for: ");
  Serial.println(SEND_EVERY);
  Serial.println();
  delay(SEND_EVERY);
}

void initSensors()
{
    Serial.println("Initializing sensors, this can take a few seconds...");
    pinMode(SoundSensorPin,INPUT);
    pinMode(LightSensorPin,INPUT);
    tph.begin();
    airqualitysensor.init(AirQualityPin);
    Serial.println("Done");
}

void ReadSensors()
{
    Serial.println("Start reading sensors");
    Serial.println("---------------------");
    soundValue = analogRead(SoundSensorPin);
    lightValue = analogRead(LightSensorPin);
    lightValue = lightValue * 3.3 / 1023;         // convert to lux, this is based on the voltage that the sensor receives
    lightValue = pow(10, lightValue);
    
    temp = tph.readTemperature();
    hum = tph.readHumidity();
    pres = tph.readPressure()/100.0;
    
    airValue = airqualitysensor.getRawData();
}

void SendSensorValues()
{
    Serial.println("Start uploading data to the ATT cloud Platform");
    Serial.println("----------------------------------------------");
    Serial.println("Sending sound value... ");
    Device.Send(soundValue, LOUDNESS_SENSOR);
    Serial.println("Sending light value... "); 
    Device.Send(lightValue, LIGHT_SENSOR);
    Serial.println("Sending temperature value... ");
    Device.Send(temp, TEMPERATURE_SENSOR);
    Serial.println("Sending humidity value... ");  
    Device.Send(hum, HUMIDITY_SENSOR);
    Serial.println("Sending pressure value... ");  
    Device.Send(pres, PRESSURE_SENSOR);
    Serial.println("Sending air quality value... ");  
    Device.Send(airValue, AIR_QUALITY_SENSOR);
}

void DisplaySensorValues()
{
    Serial.print("Sound level: ");
    Serial.print(soundValue);
	  Serial.println(" Analog (0-1023)");
      
    Serial.print("Light intensity: ");
    Serial.print(lightValue);
	  Serial.println(" Lux");
      
    Serial.print("Temperature: ");
    Serial.print(temp);
	  Serial.println(" °C");
      
    Serial.print("Humidity: ");
    Serial.print(hum);
	  Serial.println(" %");
      
    Serial.print("Pressure: ");
    Serial.print(pres);
	  Serial.println(" hPa");
  
    Serial.print("Air quality: ");
    Serial.print(airValue);
	  Serial.println(" Analog (0-1023)");
}


void serialEvent1()
{
    Device.Process();                           //for future use of actuators
}


