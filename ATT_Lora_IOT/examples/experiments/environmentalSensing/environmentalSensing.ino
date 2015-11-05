/*
AllThingsTalk - SmartLiving.io LoRa Arduino experiments
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include <Sodaq_TPH.h>
#include "AirQuality2.h"
#include "ATT_LoRa_IOT.h"
#include "keys.h"
#include "MicrochipLoRaModem.h"


#define SERIAL_BAUD 57600
#define AirQualityPin A0
#define LightSensorPin A4
#define SoundSensorPin A2

#define SEND_EVERY 1800000


MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;

unsigned long lastValueSendAt;									//the time when the last value was send	

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
  Device.SetMaxSendRetry(-1);								//for this use case we want to send the measurement or block until we have, cause that's the primary function of this device: capture and transmit the value.
  
  initTPH();
  initAirQuality();
  initLightSensor();
  initSoundSensor();
  lastValueSendAt = millis();
}

void loop() 
{
  Serial.print("delay for: ");
  Serial.println(SEND_EVERY - millis() - lastValueSendAt);
  delay(lastValueSendAt - millis() + SEND_EVERY);
  lastValueSendAt = millis();       //we start the timer before we send everything, so we send every 180.000, not every 180.000 + time it took to send prev batch.
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
  sensorValue = (float)(1023 - sensorValue) * 10 / sensorValue;
  Serial.print("light intensity: ");
  Serial.println(sensorValue);
  Device.Send(sensorValue, LIGHT_SENSOR);
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


