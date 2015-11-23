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
#include <MMA7660.h>
#include "ATT_LoRa_IOT.h"
#include "keys.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


MMA7660 accelemeter;
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  accelemeter.init();
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float accx, accy, accz;
int8_t x,y,z; 

void loop() 
{
  accelemeter.getXYZ(&x, &y, &z);
  Serial.print("values of X , Y , Z: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.println(z);
  
  accelemeter.getAcceleration(&accx, &accy, &accz);
  Serial.println("accleration of X/Y/Z: ");
  Serial.print("x: ");
	Serial.print(accx);
  Serial.print(" g, y:");
	Serial.print(accy);
	Serial.print(" g, z:");
	Serial.print(accz);
	Serial.println(" g");
	Serial.println("*************");
  //SendValue();
  
  delay(1000);
}

void SendValue()
{
  Device.Queue(accx);
  Device.Queue(accy);
  Device.Queue(accz);
  Device.Send(ACCELEROMETER);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


