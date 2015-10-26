//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "MMA7660.h"
#include <Sodaq_SHT2x.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
#include "EmbitLoRaModem.h"
//#include "MicrochipLoRaModem.h"

//#include <SoftwareSerial.h>

#define SERIAL_BAUD 57600

int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int ActionLed = 4;                                             // activated when the modem is sending a datapacket
EmbitLoRaModem Modem(&Serial1);
//MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

//reading GPS values from serial connection with GPS
//SoftwareSerial SoftSerial(2, 3);
//unsigned char buffer[64];                   // buffer array for data receive over serial port
//int count=0;  

void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  pinMode(ActionLed, OUTPUT);					           			// initialize the digital pin as an output -> show that modem is sending.          
  //SoftSerial.begin(9600); 
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  pinMode(LED1, OUTPUT);									//indicate that the device is running -> still battery power left
  digitalWrite(LED1, HIGH);
}

float longitude = 1.1;
float latitude = 2.2;
float altitude = 3.3;
float timestamp = 3.3;

void loop() 
{
  SendValue();
  longitude = longitude + 10.01;
  latitude = latitude + 10.01;
  altitude = altitude + 10.01;
  delay(300);
}

void SendValue()
{
  digitalWrite(ActionLed, 1);
  Device.Queue(longitude);
  Device.Queue(latitude);
  Device.Queue(altitude);
  Device.Queue(timestamp);
  Device.Send(GPS);
  digitalWrite(ActionLed, 0);
}

void readCoordinates()
{
//	if (SoftSerial.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
//    {
//        while(SoftSerial.available())               // reading data into char array
//        {
//            buffer[count++]=SoftSerial.read();      // writing data into array
//            if(count == 64)break;
//        }
//        Serial.write(buffer,count);                 // if no data transmission ends, write buffer to hardware serial port
//        clearBufferArray();                         // call clearBufferArray function to clear the stored data from the array
//        count = 0;                                  // set counter of while loop to zero
//    }
}

void clearBufferArray()                     // function to clear buffer array
{
//    for (int i=0; i<count;i++)
//    { buffer[i]=NULL;}                      // clear all index of array with command NULL
}

void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


