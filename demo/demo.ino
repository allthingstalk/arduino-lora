//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "MMA7660.h"
#include <Sodaq_SHT2x.h>
#include "ATT_Lora_IOT.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600



uint8_t DEV_ADDR[4] = { 0x00, 0x00, 0x13, 0x00 };
uint8_t APPSKEY[16] = { 0xef, 0xb6, 0x1c, 0x43, 0x3a, 0xa2, 0x8a, 0x9f, 0xb3, 0x46, 0xc8, 0xf9, 0x2b, 0x6b, 0x47, 0x2a };
uint8_t NWKSKEY[16] = { 0x73, 0x63, 0x98, 0x79, 0x3b, 0x0a, 0xd9, 0xce, 0x54, 0x91, 0xda, 0xe9, 0xc3, 0xdc, 0xd3, 0x85 };

int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int ActionLed = 4;                                        	   // activated when the modem is sending a datapacket
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);


bool sensorVal = false;							        //only send every x amount of time.

void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  pinMode(ActionLed, OUTPUT);					           			// initialize the digital pin as an output -> show that modem is sending.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  pinMode(LED1, OUTPUT);									//indicate that the device is running -> still battery power left
  digitalWrite(LED1, HIGH);
  
  sensorVal = digitalRead(DigitalSensor);
  SendValue(sensorVal);					//send initial state
}

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);			        // read status Digital Sensor
  if (sensorVal != sensorRead) 				                // verify if value has changed
  {
     sensorVal = sensorRead;
	   SendValue(sensorRead);
  }
  delay(100);
}

void SendValue(bool val)
{
  digitalWrite(ActionLed, 1);
  Serial.println(sensorVal);
  Device.Send(val, BINARY_SENSOR);
  digitalWrite(ActionLed, 0);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


