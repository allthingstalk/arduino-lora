/*
AllThingsTalk - LoRa Arduino demos

   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"
#include <Sodaq_DS3231.h>

//These constants are used for reading the battery voltage

#define ADC_AREF 3.3
#define BATVOLTPIN A6
#define BATVOLT_R1 4.7
#define BATVOLT_R2 10
#define SERIAL_BAUD 57600

//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);



void setup() 
{
	Serial.begin(SERIAL_BAUD);
	Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
	Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
	Serial.println("starting");
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float getRealBatteryVoltage()
{
  uint16_t batteryVoltage = analogRead(BATVOLTPIN);
  return (ADC_AREF / 1023.0) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * batteryVoltage;
} 


void loop() 
{
	//Read the temperature and display it on the OLED
	rtc.convertTemperature();            
	int temp = rtc.getTemperature();
  
    //Read the voltage and display it on the OLED
	float mv = getRealBatteryVoltage() * 1000.0;
	mv = mapfloat(mv,2.2, BATVOLT_R1, 0, 100);

	//Display the temperature reading
	Serial.print("Temp=");
	Serial.print(temp);
	Serial.println('c');

	//Display the voltage reading
	Serial.print("Volts=");
	Serial.print(mv);
	Serial.println("v");
	
	Device.Send((float)temp, TEMPERATURE_SENSOR);
	Device.Send(mv, BATTERY_LEVEL);
	
	delay(1000);
}



void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}

