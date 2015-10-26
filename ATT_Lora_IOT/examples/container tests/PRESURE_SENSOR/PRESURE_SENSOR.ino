//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//Sodaq_TPH
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries
//https://github.com/SodaqMoja/Sodaq_TPH

#include <Wire.h>
#include "MMA7660.h"
#include <Sodaq_SHT2x.h>
//#include <Sodaq_TPH.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
#include "EmbitLoRaModem.h"
//#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600



int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int ActionLed = 4;                                        	   // activated when the modem is sending a datapacket
EmbitLoRaModem Modem(&Serial1);
//MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  //tph.begin();
  pinMode(ActionLed, OUTPUT);					           			// initialize the digital pin as an output -> show that modem is sending.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  pinMode(LED1, OUTPUT);									//indicate that the device is running -> still battery power left
  digitalWrite(LED1, HIGH);
  
}

float value = 1.1;

void loop() 
{
  //int32_t pres = tph.readPressure();
  SendValue();
  value = value + 1.01;
  delay(300);
}

void SendValue()
{
  digitalWrite(ActionLed, 1);
  Serial.println(value);
  Device.Send(value, PRESURE_SENSOR);
  digitalWrite(ActionLed, 0);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


