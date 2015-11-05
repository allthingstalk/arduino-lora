/*
AllThingsTalk - SmartLiving.io LoRa Arduino demos
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include <SoftwareSerial.h>
#include "ATT_LoRa_IOT.h"
#include "keys.h"
#include "MicrochipLoRaModem.h"

//#include <SoftwareSerial.h>

#define SERIAL_BAUD 57600

MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

//reading GPS values from serial connection with GPS
SoftwareSerial SoftSerial(20, 21);
unsigned char buffer[64];                   // buffer array for data receive over serial port
int count=0;  

void setup() 
{
  SoftSerial.begin(9600); 
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float longitude;
float latitude;
float altitude;
float timestamp;

void loop() 
{
  if(readCoordinates() == true)
    SendValue();
  delay(300);
}

void SendValue()
{
  //Device.Queue(longitude);
  //Device.Queue(latitude);
  //Device.Queue(altitude);
  //Device.Queue(timestamp);
  //Device.Send(GPS);
  Serial.print("lng: ");
  Serial.print(longitude, 4);
  Serial.print(", lat: ");
  Serial.print(latitude, 4);
  Serial.print(", alt: ");
  Serial.print(altitude);
  Serial.print(", time: ");
  Serial.println(timestamp);
}

bool readCoordinates()
{
    bool foundGPGGA = false;					//sensor can return multiple types of data, need to capture lines that start with $GPGGA
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

bool ExtractValues()
{
	unsigned char start = count;
	while(buffer[start] != '$')		//find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
	{
	    if(start == 0) break;						//it's unsigned char, so we can't check on <= 0
		start--;
	}
	start++;										//remove the '$', don't need to compare with that.
	if(start + 4 < 64 && buffer[start] == 'G' && buffer[start+1] == 'P' && buffer[start+2] == 'G' && buffer[start+3] == 'G' && buffer[start+4] == 'A')		//we found the correct line, so extract the values.
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

float ExtractValue(unsigned char& start)
{
	unsigned char end = start + 1;
	while(end < count && buffer[end] != ',')		//find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
		end++;
	buffer[end] = 0;								//end the string, so we can create a string object from the sub string -> easy to convert to float.
	float result = 0.0;
	if(end != start + 1)					//if we only found a ',', then there is no value.
		result = String((const char*)(buffer + start)).toFloat();
	start = end + 1;
	return result;
}

unsigned char Skip(unsigned char start)
{
	unsigned char end = start + 1;
	while(end < count && buffer[end] != ',')		//find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
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
  Device.Process();														//for future extensions -> actuators
}


