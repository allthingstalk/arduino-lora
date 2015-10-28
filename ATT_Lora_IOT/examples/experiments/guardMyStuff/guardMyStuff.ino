/*
AllThingsTalk - SmartLiving.io LoRa Arduino experiments
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/


#include <Wire.h>
#include <SoftwareSerial.h>
#include <ADXL345.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600
#define MOVEMENTTRESHOLD 10         //amount of movement that can be detected before being considered as really moving (jitter on the accelerometer)

//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

ADXL345 accelemeter;
SoftwareSerial SoftSerial(20, 21);          //reading GPS values from serial connection with GPS
unsigned char buffer[64];                   // buffer array for data receive over serial port
int count=0;  

//variables for the coordinates (GPS)
float longitude;
float latitude;
float altitude;
float timestamp;

int prevX,prevY,prevZ;                          //keeps track of the accelerometer data that was read last previosly, so we can detect a difference in position.
unsigned long prevCoordinatesAt;                //only send the coordinates every 15 seconds, so we need to keep track of the time.

//accelerometer data is translated to 'moving vs not moving' on the device.
//This value is sent to the cloud using a 'push-button' container. 
bool wasMoving = false;                         //at startup, we presume that the device is not moving, so we can pick 

void setup() 
{
  accelemeter.powerOn();                                        //accelerometer is always running, so we can check when the object is moving around or not.
  SoftSerial.begin(9600); 
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    //init the baud rate of the serial connection so that it's ok for the modem
  
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))
  {
    Serial.println("retrying...");
    delay(200);
  }
  Device.Send(false, PUSH_BUTTON);                              //we have started up, make certain that the cloud is initialized with the state of the device (in case the device was turned off last time while it was moving).
  
  accelemeter.readXYZ(&prevX, &prevY, &prevZ);                  //get the current state of the accelerometer, so we can use this info in the loop as something to compare against.
}

void loop() 
{
  if(isMoving() == true)
  {
      if(wasMoving == false)
      {
          Serial.println("movement detected");
          //optional improvement: only turn on the gps when it is used: while the device is moving.
          wasMoving = true;
          Device.Send(true, PUSH_BUTTON);
      }
      if(prevCoordinatesAt < millis() - 15000)          //only send every 15 seconds, so we don't swamp the system.
      {
		 Serial.print("prev time: "); Serial.print(prevCoordinatesAt); Serial.print("cur time: "); Serial.println(millis());
         while(readCoordinates() == false) delay(300);  //try to read some coordinates until we have a valid set. Everyt time we fail, pause a little to give the GPS some time.
         SendCoordinates();                            //send the coordinates over.
         prevCoordinatesAt = millis(); 
      }
  }
  else if(wasMoving == true)
  {
     Serial.println("movement stopped");
     //we don't need to send coordinates when the device has stopped moving -> they will always be the same, so we can save some power.
     //optional improvement: turn off the gps module
     wasMoving = false;
     Device.Send(false, PUSH_BUTTON);
  }
  delay(1000);
}

bool isMoving()
{
  int x,y,z;
  accelemeter.readXYZ(&x, &y, &z);
  Serial.print("x: "); Serial.print(x); Serial.print(", y: "); Serial.print(y); Serial.print(", z: "); Serial.println(z);
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > MOVEMENTTRESHOLD;
  prevX = x;
  prevY = y;
  prevZ = z;
  return result;
}

//sends the GPS coordinates to the cloude
void SendCoordinates()
{
  Device.Queue(longitude);
  Device.Queue(latitude);
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

//tries to read the gps coordinates from the text stream that was received from the gps module.
//returns: true when gps coordinates were found in the input, otherwise false.
bool readCoordinates()
{
    bool foundGPGGA = false;                    //sensor can return multiple types of data, need to capture lines that start with $GPGGA
    if (SoftSerial.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
    {
        while(SoftSerial.available())               // reading data into char array
        {
            buffer[count++]=SoftSerial.read();      // writing data into array
            if(count == 64)break;
        }
        //Serial.println(count);
        foundGPGGA = count > 60 && ExtractValues();  //if we have less then 60 characters, then we have bogus input, so don't try to parse it or process the values
        clearBufferArray();                         // call clearBufferArray function to clear the stored data from the array
        count = 0;                                  // set counter of while loop to zero
    }
    return foundGPGGA;
}

//extraxts all the coordinates from the stream that was received from the module 
//and stores the values in the globals defined at the top of the sketch.
bool ExtractValues()
{
    unsigned char start = count;
    while(buffer[start] != '$')     //find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
    {
        if(start == 0) break;                       //it's unsigned char, so we can't check on <= 0
        start--;
    }
    start++;                                        //remove the '$', don't need to compare with that.
    if(start + 4 < 64 && buffer[start] == 'G' && buffer[start+1] == 'P' && buffer[start+2] == 'G' && buffer[start+3] == 'G' && buffer[start+4] == 'A')      //we found the correct line, so extract the values.
    {
        start+=6;
        timestamp = ExtractValue(start);
        longitude = ExtractValue(start) / 100;
        start = Skip(start);    
        latitude = ExtractValue(start)  / 100;
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

//exracts a single value out of the stream received from the device and returns this value.
float ExtractValue(unsigned char& start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')        //find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
        end++;
    buffer[end] = 0;                                //end the string, so we can create a string object from the sub string -> easy to convert to float.
    float result = 0.0;
    if(end != start + 1)                    //if we only found a ',', then there is no value.
        result = String((const char*)(buffer + start)).toFloat();
    start = end + 1;
    return result;
}

//skips a position in the text stream that was received from the gps.
unsigned char Skip(unsigned char start)
{
    unsigned char end = start + 1;
    while(end < count && buffer[end] != ',')        //find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
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
  Device.Process();                                                     //for future extensions -> actuators
}


