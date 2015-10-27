//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include <ADXL345.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


ADXL345 accelemeter;
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  accelemeter.powerOn();
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

double xyz[3];
int x,y,z; 

void loop() 
{
  accelemeter.readXYZ(&x, &y, &z);
  Serial.print("values of X , Y , Z: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.println(z);
  
  accelemeter.getAcceleration(xyz);
  Serial.println("accleration of X/Y/Z: ");
  Serial.print("x: ");
	Serial.print(xyz[0]);
  Serial.print(" g, y:");
	Serial.print(xyz[1]);
	Serial.print(" g, z:");
	Serial.print(xyz[2]);
	Serial.println(" g");
	Serial.println("*************");
  //SendValue();
  
  delay(1000);
}

void SendValue()
{
  Device.Queue((float)xyz[0]);
  Device.Queue((float)xyz[1]);
  Device.Queue((float)xyz[2]);
  Device.Send(ACCELEROMETER);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


