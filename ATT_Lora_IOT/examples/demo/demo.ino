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
#include <ATT_LoRa_IOT.h>
//#include "EmbitLoRaMode.h"
#include <MicrochipLoRaModem.h>
#include "keys.h"

#define SERIAL_BAUD 57600

int DigitalSensor = 20;                              // pin this sensor is connected to
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

bool sensorVal = false;
bool prevButtonState = false;

void setup() 
{
  pinMode(DigitalSensor, INPUT);               // initialize the digital pin as an input
  Serial.begin(SERIAL_BAUD);                   // set baud rate of the default serial debug connection
  Serial1.begin(Modem.getDefaultBaudRate());   // set baud rate of the serial connection between Mbili and LoRa modem
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))
	Serial.println("retrying...");						// initialize connection with the AllThingsTalk Developer Cloud
  Device.SetMaxSendRetry(0);
  Device.SetMinTimeBetweenSend(0);
  Serial.println("Ready to send data");

  sensorVal = digitalRead(DigitalSensor);
  SendValue(sensorVal);                        // send initial state
}

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);      // read status Digital Sensor
  if (sensorRead == 1 && prevButtonState == false)                               // verify if value has changed
  {
    prevButtonState = true;
    if(SendValue(!sensorVal) == true)
		  sensorVal = !sensorVal;
  }
  else if(sensorRead == 0)
    prevButtonState = false;
  delay(100);
}

bool SendValue(bool val)
{
  Serial.print("Data: ");Serial.println(val);
  bool res = Device.Send(val, BINARY_SENSOR);
  if(res == false)
    Serial.println("maybe the last couple of packages were sent too quickly after each other? (min of 15 seconds recommended)");
  return res;
}

void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}
