/*
AllThingsTalk - SmartLiving.io LoRa Arduino demos
Released into the public domain.

Original author: Jan Bogaerts (2015)
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

void setup() 
{
  pinMode(DigitalSensor, INPUT);               // initialize the digital pin as an input
  Serial.begin(SERIAL_BAUD);                   // set baud rate of the default serial debug connection
  Serial1.begin(Modem.getDefaultBaudRate());   // set baud rate of the serial connection between Mbili and LoRa modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);  // initialize connection with the AllThingsTalk Developer Cloud
  Serial.println("Ready to send data");

  sensorVal = digitalRead(DigitalSensor);
  SendValue(sensorVal);                        // send initial state
}

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);      // read status Digital Sensor
  if (sensorRead == 1)                               // verify if value has changed
  {
    sensorVal = !sensorVal;
    SendValue(sensorVal);
  }
  delay(100);
}

void SendValue(bool val)
{
  Serial.println(val);
  Device.Send(val, BINARY_SENSOR);
}

void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}
