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
#include <SoftwareSerial.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);

// reading GPS values from serial connection with GPS
SoftwareSerial SoftSerial(20, 21);
unsigned char buffer[64];                   // buffer array for data receive over serial port
int count=0;  

void setup() 
{
  SoftSerial.begin(9600); 
  while((!Serial) && (millis()) < 2000){}				//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());       		// init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  Serial.println();
}

float longitude;
float latitude;
float altitude;
float timestamp;

void loop() 
{
  if(readCoordinates() == true)
    SendValue();
  delay(3000);
}

void SendValue()
{
  Device.Queue(latitude);
  Device.Queue(longitude);
  Device.Queue(altitude);
  Device.Queue(timestamp);
  Device.Send(GPS);
  Serial.print("Coordinates: ");
  Serial.print(latitude, 4);
  Serial.print(",");
  Serial.println(longitude, 4);
  Serial.print("Altitude: ");
  Serial.println(altitude, 4);
  Serial.print("Timestamp: ");
  Serial.println(timestamp);
  Serial.println();
}

bool readCoordinates()
{
  bool foundGPGGA = false;                          // sensor can return multiple types of data, need to capture lines that start with $GPGGA
    if (SoftSerial.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
    {
        while(SoftSerial.available())               // reading data into char array
        {
            buffer[count++]=SoftSerial.read();      // writing data into array
            if(count == 64)break;
        }
        //Serial.println(count); 
        //Serial.println((char*)buffer);
        foundGPGGA = count > 60 && ExtractValues();  // if we have less then 60 characters, then we have bogus input, so don't try to parse it or process the values
        clearBufferArray();                          // call clearBufferArray function to clear the stored data from the array
        count = 0;                                   // set counter of while loop to zero
    }
    return foundGPGGA;
}

bool ExtractValues()
{
    unsigned char start = count;
    while(buffer[start] != '$')     // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
    {
        if(start == 0) break;                       // it's unsigned char, so we can't check on <= 0
          start--;
    }
    start++;                                        // remove the '$', don't need to compare with that.
    if(start + 4 < 64 && buffer[start] == 'G' && buffer[start+1] == 'P' && buffer[start+2] == 'G' && buffer[start+3] == 'G' && buffer[start+4] == 'A')      //we found the correct line, so extract the values.
    {
    start+=6;
        timestamp = ExtractValue(start);
        latitude = ConvertDegrees(ExtractValue(start) / 100);
        start = Skip(start);    
        longitude = ConvertDegrees(ExtractValue(start)  / 100);
        start = Skip(start);
        start = Skip(start);
        start = Skip(start);
        start = Skip(start);
        altitude = ExtractValue(start);
        return true;
    }
    else
        return false;
}

float ExtractValue(unsigned char& start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')      // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
        end++;
    buffer[end] = 0;                              // end the string, so we can create a string object from the sub string -> easy to convert to float.
    float result = 0.0;
    if(end != start + 1)                          // if we only found a ',', then there is no value.
        result = String((const char*)(buffer + start)).toFloat();
    start = end + 1;
    return result;
}

float ConvertDegrees(float input)
{
    float fractional = input - (int)input;
    return (int)input + (fractional / 60.0) * 100.0;
}


unsigned char Skip(unsigned char start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')        // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
        end++;
    return end+1;
}

void clearBufferArray()                     // function to clear buffer array
{
    for (int i=0; i<count;i++)
    { buffer[i]=NULL;}                      // clear all index of array with command NULL
}

void serialEvent1()
{
  Device.Process();                         // for future use of actuators
}


