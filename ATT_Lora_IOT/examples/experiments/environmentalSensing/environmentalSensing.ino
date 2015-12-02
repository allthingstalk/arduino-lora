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
  
  initTPH();
  initAirQuality();
  initLightSensor();
  initSoundSensor();
}

void loop() 
{
  Serial.print("delay for: ");
  Serial.println(SEND_EVERY);
  delay(SEND_EVERY);
  processTPH();
  processAirQuality();
  processLightSensor();
  processSoundSensor();
}

void initSoundSensor()
{
  pinMode(SoundSensorPin,INPUT);
  processSoundSensor();
}

void processSoundSensor()
{
  float sensorValue = analogRead(SoundSensorPin);
  Serial.print("sound level: ");
  Serial.println(sensorValue);
  Device.Send(sensorValue, LOUDNESS_SENSOR);
}

void initLightSensor()
{
  pinMode(LightSensorPin,INPUT);
  processLightSensor();
}

void processLightSensor()
{
  float sensorValue = analogRead(LightSensorPin);
  float Rsensor= sensorValue * 3.3 / 1023;							//convert to lux, this is based on the voltage that the sensor receives
  Rsensor = pow(10, Rsensor);
  Serial.print("light intensity: ");
  Serial.println(Rsensor);
  Device.Send(Rsensor, LIGHT_SENSOR);
}

void initTPH()
{
  tph.begin();
  processTPH();
}

void processTPH()
{
  float temp = tph.readTemperature();
  float hum = tph.readHumidity();
  int32_t pres = tph.readPressure();
  
  Serial.print("temp: ");
  Serial.print(temp);
  
  Serial.print(", humidity: ");
  Serial.print(hum);
  
  Serial.print(", pressure: ");
  Serial.print(pres);
  Serial.println();

  Device.Send(temp, TEMPERATURE_SENSOR);
  Device.Send(hum, HUMIDITY_SENSOR);
  Device.Send((short)pres, PRESSURE_SENSOR);
}

void initAirQuality()
{
  airqualitysensor.init(AirQualityPin);
  processAirQuality();
}


void processAirQuality()
{
    short value = airqualitysensor.getRawData();
    Serial.print("air quality: ");
    Serial.println(value);
    Device.Send(value, AIR_QUALITY_SENSOR);
}


void serialEvent1()
{
  Device.Process();                                                     //for future extensions -> actuators
}


