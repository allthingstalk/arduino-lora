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
#include "AirQuality2.h"
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>


#define SERIAL_BAUD 57600
#define AirQualityPin A0
#define LightSensorPin A4
#define SoundSensorPin A2

#define SEND_EVERY 3000000


MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;

void setup() 
{
  pinMode(GROVEPWR, OUTPUT);                                    //turn the power on on the secondary row of grove connectors.
  digitalWrite(GROVEPWR, HIGH);

  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    //init the baud rate of the serial connection so that it's ok for the modem
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))            //there is no point to continue if we can't connect to the cloud: this device's main purpose is to send data to the cloud.
  {
    Serial.println("retrying...");
    delay(200);
  }
  Device.SetMaxSendRetry(-1);                                   //for this use case we want to send the measurement or block until we have, cause that's the primary function of this device: capture and transmit the value.
  
  initSensors();
}

void loop() 
{
  Serial.print("delay for: ");
  Serial.println(SEND_EVERY);
  ReadSensors();
  DisplaySensors();
  SendSensors();
  delay(SEND_EVERY);
}

void initSensors()
{
	pinMode(SoundSensorPin,INPUT);
	pinMode(LightSensorPin,INPUT);
	tph.begin();
	airqualitysensor.init(AirQualityPin);
}

void ReadSensors()
{
	soundValue = analogRead(SoundSensorPin);
	
	lightValue = analogRead(LightSensorPin);
    lightValue = lightValue * 3.3 / 1023;							//convert to lux, this is based on the voltage that the sensor receives
    lightValue = pow(10, lightValue);
	
	temp = tph.readTemperature();
    hum = tph.readHumidity();
    pres = tph.readPressure();
	
	airValue = airqualitysensor.getRawData();
}

void SendSensors()
{
	Device.Send(soundValue, LOUDNESS_SENSOR);
	Device.Send(lightValue, LIGHT_SENSOR);
	Device.Send(temp, TEMPERATURE_SENSOR);
    Device.Send(hum, HUMIDITY_SENSOR);
    Device.Send(pres, PRESSURE_SENSOR);
	Device.Send(airValue, AIR_QUALITY_SENSOR);
}

void DisplaySensors()
{
	Serial.print("sound level: ");
	Serial.println(soundValue);
	  
	Serial.print("light intensity: ");
	Serial.println(lightValue);
	  
	Serial.print("temp: ");
	Serial.print(temp);
	  
	Serial.print(", humidity: ");
	Serial.print(hum);
	  
	Serial.print(", pressure: ");
	Serial.print(pres);
	Serial.println();
  
    Serial.print("air quality: ");
    Serial.println(airValue);
}


void serialEvent1()
{
	Device.Process();                                                     //for future extensions -> actuators
}


