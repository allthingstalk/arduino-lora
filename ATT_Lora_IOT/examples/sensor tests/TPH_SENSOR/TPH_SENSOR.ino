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
#include <Sodaq_TPH.h>
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>


#define SERIAL_BAUD 57600


MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  tph.begin();
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");  
}

void loop() 
{
  float temp = tph.readTemperature();
  float bmp_temp = tph.readTemperatureBMP();
  float sht_temp = tph.readTemperatureSHT();
  float hum = tph.readHumidity();
  float pres = tph.readPressure();
  
  Serial.print("temp: ");
  Serial.print(temp);
  
  
  Serial.print(", temp_BMP: ");
  Serial.print(bmp_temp);
  
  Serial.print(", temp_sht: ");
  Serial.print(sht_temp);
  
  Serial.print(", humidity: ");
  Serial.print(hum);
  
  
  Serial.print(", pressure: ");
  Serial.print(pres);
  Serial.println();

  Device.Send(temp, TEMPERATURE_SENSOR);
  Device.Send(hum, HUMIDITY_SENSOR);
  Device.Send(pres, PRESSURE_SENSOR);
  
  delay(3000);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


