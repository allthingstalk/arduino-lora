//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600

int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);


void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  pinMode(LED1, OUTPUT);									//indicate that the device is running -> still battery power left
  digitalWrite(LED1, HIGH);
 
}

bool sensorVal = true;

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);			        // read status Digital Sensor
  Serial.println(sensorRead);
  if (sensorVal != sensorRead) 				                // verify if value has changed
  {
     sensorVal = sensorRead;
	   //SendValue(sensorRead);
	   if(sensorVal == true)
		   Serial.println("door closed");
	   else
		    Serial.println("door open");
  }
  delay(100);
}

void SendValue(bool val)
{
  Serial.println(val);
  Device.Send(val, DOOR_SENSOR);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}



