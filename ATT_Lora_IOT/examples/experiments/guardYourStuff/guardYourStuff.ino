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
 * If the accelerometer senses movement, the device will send out GPS coordinates. If we have our lock turned on,
 * movement will trigger a notification which will be visible in the AllthingsTalk Developer Cloud and on our Smartphone.
 * If the lock is turned off, nothing will happen, regardless of any movement of the device.
 *
 **/
 



#include <Wire.h>
#include <SoftwareSerial.h>
#include <MMA7660.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600
#define MOVEMENTTRESHOLD 12                             // amount of movement that can be detected before being considered as really moving (jitter on the accelerometer, vibrations)
#define GPS_DATA_EVERY 15000                            // the amount of time between 2 consecutive GPS updates while moving
long gpsLastSentAt = 0;                                 // keeps track of the last time that we sent over GPS coordinates

MicrochipLoRaModem Modem(&Serial1, &Serial);
ATTDevice Device(&Modem, &Serial);

MMA7660 accelemeter;
SoftwareSerial SoftSerial(20, 21);                      // reading GPS values from serial connection with GPS
unsigned char buffer[64];                               // buffer array for data receive over serial port
int count=0;  

// Variables for the coordinates (GPS)
float latitude;
float longitude;
float altitude;
float timestamp;

int8_t prevX,prevY,prevZ;                               // keeps track of the accelerometer data that was read last previously, so we can detect a difference in position

// Accelerometer data is translated to 'moving vs not moving' on the device.
// This boolean value is sent to the cloud using a generic 'Binary Sensor' container. 
bool wasMoving = false;                                         

void setup() 
{
  accelemeter.init();                                   // accelerometer is always running so we can check when the object is moving around or not
  SoftSerial.begin(9600); 
  while((!Serial) && (millis()) < 2000){}				//wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());            // init the baud rate of the serial connection so that it's ok for the modem

  Serial.println("-- Guard your stuff LoRa experiment --");
  Serial.print("Initializing GPS");
  while(readCoordinates() == false){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Done");
  Serial.println("Initializing LoRA modem");
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))
  {
    Serial.println("Retrying...");
    delay(200);
  }
  accelemeter.getXYZ(&prevX, &prevY, &prevZ);          // get the current state of the accelerometer so we can use this info in the loop as something to compare against
  Serial.println("Sending initial state");
  Device.Send(false, BINARY_SENSOR);
  Serial.println("Ready to guard your stuff");
  Serial.println();
}

void loop() 
{
  if(isMoving() == true)
  {   
      if(wasMoving == false)
      {
          Serial.println();
          Serial.println("Movement detected");
          Serial.println("-----------------");
          //optional improvement: only turn on the gps when it is used: while the device is moving
          wasMoving = true;
          Device.Send(true, BINARY_SENSOR);
          gpsLastSentAt =  millis();                  // block the 1st transmission of gps data for a short period, so we don't get timeouts from the lib/basestation
      }
      if(gpsLastSentAt + GPS_DATA_EVERY < millis()){
        SendCoordinates();                            // send the coordinates over
        gpsLastSentAt = millis();
      }
  }
  else if(wasMoving == true)
  {
     Serial.println();
     Serial.println("Movement stopped");
     Serial.println("----------------");
    // we don't need to send coordinates when the device has stopped moving -> they will always be the same, so we can save some power.
    // optional improvement: turn off the gps module
    wasMoving = false;
    Device.Send(false, BINARY_SENSOR);
    SendCoordinates();                               // send over last known coordinates
  }
  delay(500);                                        // sample the accelerometer quickly -> not so costly.
}

bool isMoving()
{
  int8_t x,y,z;
  accelemeter.getXYZ(&x, &y, &z);
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > MOVEMENTTRESHOLD;

  if(result == false && wasMoving == true){         // when movment stops, we check 2 times, cause often, the accelerometer reports for a single tick that movement has stopped, but it hasn't, the distance between the 2 measurement points was just too close, so remeasure, make certain that movement has really stopped
    delay(800);
    accelemeter.getXYZ(&x, &y, &z);
    result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > MOVEMENTTRESHOLD;
  }
  
  if(result == true){
    prevX = x;
    prevY = y;
    prevZ = z;
  }

  return result; 
}

//tries to read the gps coordinates from the text stream that was received from the gps module.
//returns: true when gps coordinates were found in the input, otherwise false.
bool readCoordinates()
{
    bool foundGPGGA = false;                       // sensor can return multiple types of data, need to capture lines that start with $GPGGA
    if (SoftSerial.available())                     
    {
        while(SoftSerial.available())              // reading data into char array
        {
            buffer[count++]=SoftSerial.read();     // store the received data in a temp buffer for further processing later on
            if(count == 64)break;
        }
        foundGPGGA = count > 60 && ExtractValues();        // if we have less then 60 characters, then we have bogus input, so don't try to parse it or process the values
        clearBufferArray();                                // call clearBufferArray function to clear the stored data from the array
    }
    return foundGPGGA;
}

//sends the GPS coordinates to the cloude
void SendCoordinates()
{
  Serial.print("Retrieving GPS coordinates for transmission, please wait");
  while(readCoordinates() == false){             // try to read some coordinates until we have a valid set. Every time we fail, pause a little to give the GPS some time. There is no point to continue without reading gps coordinates. The bike was potentially stolen, so the lcoation needs to be reported before switching back to none moving.
      Serial.print(".");
      delay(1000);                 
  }
  Serial.println();
         
  Device.Queue(latitude);
  Device.Queue(longitude);
  Device.Queue(altitude);
  Device.Queue(timestamp);
  Device.Send(GPS);
  Serial.print("lng: ");
  Serial.print(longitude, 4);
  Serial.print(", lat: ");
  Serial.print(latitude, 4);
  Serial.print(", alt: ");
  Serial.print(altitude);
  Serial.print(", time: ");
  Serial.println(timestamp);
}


//extraxts all the coordinates from the stream that was received from the module 
//and stores the values in the globals defined at the top of the sketch.
bool ExtractValues()
{
    unsigned char start = count;
    while(buffer[start] != '$')                  // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
    {
        if(start == 0) break;                    // it's unsigned char, so we can't check on <= 0
        start--;
    }
    start++;                                     // remove the '$', don't need to compare with that.
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

float ConvertDegrees(float input)
{
    float fractional = input - (int)input;
    return (int)input + (fractional / 60.0) * 100.0;
}


// extracts a single value out of the stream received from the device and returns this value.
float ExtractValue(unsigned char& start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')        // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
        end++;
    buffer[end] = 0;                                // end the string, so we can create a string object from the sub string -> easy to convert to float.
    float result = 0.0;
    if(end != start + 1)                            // if we only found a ',', then there is no value.
        result = String((const char*)(buffer + start)).toFloat();
    start = end + 1;
    return result;
}

//skips a position in the text stream that was received from the gps.
unsigned char Skip(unsigned char start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')       // find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
        end++;
    return end+1;
}

void clearBufferArray()                           // function to clear buffer array
{
    for (int i=0; i<count;i++) buffer[i]=NULL;    // reset the entire buffer back to 0
    count = 0;
}

void serialEvent1()
{
  Device.Process();                               // for future extensions -> actuators
}
