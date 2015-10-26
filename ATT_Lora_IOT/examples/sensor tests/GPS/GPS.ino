//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include <SoftwareSerial.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

//#include <SoftwareSerial.h>

#define SERIAL_BAUD 57600

//EmbitLoRaModem Modem(&Serial1);
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

float longitude = 1.1;
float latitude = 2.2;
float altitude = 3.3;
float timestamp = 3.3;

void loop() 
{
  readCoordinates();
  //SendValue();
  delay(300);
}

void SendValue()
{
  Device.Queue(longitude);
  Device.Queue(latitude);
  Device.Queue(altitude);
  Device.Queue(timestamp);
  Device.Send(GPS);
}

void readCoordinates()
{
	if (SoftSerial.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
    {
		bool foundGPGGA = false;					//sensor can return multiple types of data, need to capture lines that start with $GPGGA
		while(foundGPGGA == false)
        {
			while(SoftSerial.available())               // reading data into char array
			{
				buffer[count++]=SoftSerial.read();      // writing data into array
				if(count == 64)break;
			}
			Serial.write(buffer,count);                 // if no data transmission ends, write buffer to hardware serial port
			foundGPGGA = ExtractValues();
			clearBufferArray();                         // call clearBufferArray function to clear the stored data from the array
			count = 0;                                  // set counter of while loop to zero
		}
    }
	
}

bool ExtractValues()
{
	unsigned char start = count;
	while(start >= 0 && buffer[start] != '$')		//find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
		start--;
	start++;										//remove the '$', don't need to compare with that.
	if(strcmp(buffer + start, "GPGGA") == 0)		//we found the correct line, so extract the values.
	{
		unsigned char end;
		timestamp = ExtractValue(start, end);
		start = end + 1;
		longitude = ExtractValue(start, end);
		end = Skip(start + 1);
		latitude = ExtractValue(start, end);
		end = Skip(start + 1);
		end = Skip(start + 1);
		end = Skip(start + 1);
		altitude = ExtractValue(start, end);
		start = end + 1;
		return true;
	}
	else
		return false;
}

float ExtractValue(unsigned char start, unsigned char& end)
{
	end = start + 1;
	while(end < count && buffer[end] != ',')		//find the start of the GPS data -> multiple $GPGGA can appear in 1 line, if so, need to take the last one.
		end++;
	buffer[end] = 0;								//end the string, so we can create a string object from the sub string -> easy to convert to float.
	return String(buffer + start).toFloat();
}

unsigned char Skip(unsigned char start)
{
	end = start + 1;
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


