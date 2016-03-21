/****
 *  AllThingsTalk Developer Cloud IoT experiment for LoRa
 *  Version 1.0 dd 09/11/2015
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
 *  
 **/

 /**** 
  *  values ranges
  *  
  * 0-300 is good air quality
  * 300-700 is polluted
  * above 700 is highly polluted
  **/

#include <Wire.h>
#include "AirQuality2.h"
#include"Arduino.h"
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600


#define AnalogSensor A4

MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;
short value;

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					// init the baud rate of the serial connection so that it's ok for the modem
  airqualitysensor.init(AnalogSensor);
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

void loop() 
{
  value = airqualitysensor.getRawData();
  SendValue();
  delay(3000);
}

void SendValue()
{
  Serial.print("Air quality: ");
  Serial.print(value);
  Serial.println("   Analog (0-1023)");
  Serial.print("Sending");
  Device.Send(value, AIR_QUALITY_SENSOR, false);
}


void serialEvent1()
{
  Device.Process();														// for future use of actuators
}


