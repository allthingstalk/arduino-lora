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
#include <Sodaq_TPH.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>


#define SERIAL_BAUD 57600


MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);


void setup() 
{
  tph.begin();                                        // connect TPH sensor to the I2C pin (SCL/SDA)
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					// init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");  
}

void loop() 
{
  float temp = tph.readTemperature();
  float bmp_temp = tph.readTemperatureBMP();
  float sht_temp = tph.readTemperatureSHT();
  float hum = tph.readHumidity();
  float pres = tph.readPressure()/100.0;
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");
  
  Serial.print("Temperature (BMP sensor): ");
  Serial.print(bmp_temp);
  Serial.println(" °C");
  
  Serial.print("Temperature (SHT sensor): ");
  Serial.print(sht_temp);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  
  Serial.print("Pressure: ");
  Serial.print(pres);
  Serial.println(" hPa");
  Serial.println();

  Device.Send(temp, TEMPERATURE_SENSOR);
  Device.Send(hum, HUMIDITY_SENSOR);
  Device.Send(pres, PRESSURE_SENSOR);
  
  delay(3000);
}


void serialEvent1()
{
  Device.Process();														// for future use of actuators
}


