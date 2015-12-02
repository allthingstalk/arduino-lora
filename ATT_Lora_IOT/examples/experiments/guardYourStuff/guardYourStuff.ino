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
 *  -> http://docs.smartliving.io/kits/lora
 */


#include <Wire.h>
#include <SoftwareSerial.h>
#include <MMA7660.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600
#define MOVEMENTTRESHOLD 20                             //amount of movement that can be detected before being considered as really moving (jitter on the accelerometer)
#define NO_MOVE_DELAY 60000                             //amount of time that the accelerometer must report 'no movement' before actually changes state to 'not moving', this is for capturing short stand-stills, like a red-light. -> for commercial products, do time *10
#define GPS_DATA_EVERY 90000                            //the amount of time between 2 consecutive GPS updates while moving. -> for commercial products, do time * 10

MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

MMA7660 accelemeter;
SoftwareSerial SoftSerial(20, 21);                      //reading GPS values from serial connection with GPS
unsigned char buffer[64];                               // buffer array for data receive over serial port
int count=0;  

//variables for the coordinates (GPS)
float longitude;
float latitude;
float altitude;
float timestamp;

int8_t prevX,prevY,prevZ;                              //keeps track of the accelerometer data that was read last previosly, so we can detect a difference in position.
unsigned long prevCoordinatesAt;                        //only send the coordinates every 15 seconds, so we need to keep track of the time.

//accelerometer data is translated to 'moving vs not moving' on the device (fog computing).
//This value is sent to the cloud using a 'push-button' container. 
bool wasMoving = false;                                         
bool wasMovingDelay = false;                                            //we delay the switch to 'wasMoving = false' for 10 minutes, in order to compensate for short stops (ex: stopping at a traffic light). This cuts down on the number of messages that we send (expensive)
unsigned long movemementStoppedAt;                                      //the moment that movement stopped, so we can add a delay of 10 minutes (amount of time that there can't be any movement) before actually changing the state to 'none-moving'.

void setup() 
{
  accelemeter.init();                                                   //accelerometer is always running, so we can check when the object is moving around or not.
  SoftSerial.begin(9600); 
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                            //init the baud rate of the serial connection so that it's ok for the modem
  
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))
  {
    Serial.println("retrying...");
    delay(200);
  }
  accelemeter.getXYZ(&prevX, &prevY, &prevZ);                           //get the current state of the accelerometer, so we can use this info in the loop as something to compare against.
}

void loop() 
{
  if(isMoving() == true)
  {
      wasMovingDelay = false;                                           //reset this flag every time that movement is detected, so that we are prepared to capture a delay to move 'wasMoving' back to false.
      if(wasMoving == false)
      {
          Serial.println("movement detected");
          //optional improvement: only turn on the gps when it is used: while the device is moving.
          wasMoving = true;
          Device.Send(true, PUSH_BUTTON);
          prevCoordinatesAt = millis();                                 //when movement begins, we always wait for 'GPS_DATA_EVERY' amount of time before sending the first gps coordinates (unless movement stopped earlier)
      }
      if(prevCoordinatesAt + GPS_DATA_EVERY <= millis())                //only send every 15 seconds, so we don't swamp the system.
      {
         SendCoordinates();                                             //send the coordinates over.
         prevCoordinatesAt = millis();
      }
  }
  else if(wasMoving == true)
  {
     if(wasMovingDelay == false)
     {
        movemementStoppedAt = millis();
        wasMovingDelay = true;
     }
     else if(movemementStoppedAt + NO_MOVE_DELAY <= millis())           //only change the state when the delay period has passed.
     {
        Serial.println("movement stopped");
        //we don't need to send coordinates when the device has stopped moving -> they will always be the same, so we can save some power.
        //optional improvement: turn off the gps module
        wasMoving = false;
        SendCoordinates();                                              //when we have stopped moving, best to report the final destination point.
    }
  }
  delay(100);                                                           //sample the accelerometer quickly -> not so costly.
}

bool isMoving()
{
  int8_t x,y,z;
  accelemeter.getXYZ(&x, &y, &z);
  //Serial.print("x: "); Serial.print(x); Serial.print(", y: "); Serial.print(y); Serial.print(", z: "); Serial.println(z);
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > MOVEMENTTRESHOLD;
  prevX = x;
  prevY = y;
  prevZ = z;
  return result;
}

//sends the GPS coordinates to the cloude
void SendCoordinates()
{
  Serial.print("prev time: "); Serial.print(prevCoordinatesAt); Serial.print("cur time: "); Serial.println(millis());
  while(readCoordinates() == false) delay(300);                 //try to read some coordinates until we have a valid set. Every time we fail, pause a little to give the GPS some time. There is no point to continue without reading gps coordinates. The bike was potentially stolen, so the lcoation needs to be reported before switching back to none moving.
         
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

//tries to read the gps coordinates from the text stream that was received from the gps module.
//returns: true when gps coordinates were found in the input, otherwise false.
bool readCoordinates()
{
    bool foundGPGGA = false;                                    //sensor can return multiple types of data, need to capture lines that start with $GPGGA
    if (SoftSerial.available())                     
    {
        while(SoftSerial.available())                           // reading data into char array
        {
            buffer[count++]=SoftSerial.read();                  // store the received data in a temp buffer for further processing later on
            if(count == 64)break;
        }
        foundGPGGA = count > 60 && ExtractValues();             //if we have less then 60 characters, then we have bogus input, so don't try to parse it or process the values
        clearBufferArray();                                     // call clearBufferArray function to clear the stored data from the array
    }
    return foundGPGGA;
}

//extraxts all the coordinates from the stream that was received from the module 
//and stores the values in the globals defined at the top of the sketch.
bool ExtractValues()
{
    unsigned char start = count;
    while(buffer[start] != '$')                                 //find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
    {
        if(start == 0) break;                                   //it's unsigned char, so we can't check on <= 0
        start--;
    }
    start++;                                                    //remove the '$', don't need to compare with that.
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

void clearBufferArray()                             // function to clear buffer array
{
    for (int i=0; i<count;i++) buffer[i]=NULL;      // reset the entire buffer back to 0
    count = 0;
}

void serialEvent1()
{
  Device.Process();                                                     //for future extensions -> actuators
}


