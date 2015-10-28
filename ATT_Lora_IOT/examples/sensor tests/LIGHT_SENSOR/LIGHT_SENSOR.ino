/*
AllThingsTalk - SmartLiving.io LoRa Arduino demos
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600

#define AnalogSensor A5


//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  pinMode(AnalogSensor,INPUT);
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float value = 0;

void loop() 
{
  int sensorValue = analogRead(AnalogSensor); 
  float Rsensor=(float)(1023-sensorValue)*10/sensorValue;
  Serial.println(Rsensor);
  if((int)value != (int)Rsensor){			//we round it off, so we don't send to many value changes
    value = Rsensor;
    SendValue();
  }
  delay(300);
}

void SendValue()
{
  Serial.print("Sending value: ");
  Serial.println(value);
  Device.Send(value, LIGHT_SENSOR);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


