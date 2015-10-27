#include <Wire.h>
#include <Sodaq_TPH.h>
#include "AirQuality2.h"
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"


#define SERIAL_BAUD 57600
#define AirQualityPin A0
#define LightSensorPin A4
#define SoundSensorPin A2


//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;

int prevAirQuality;
float prevTemp;
float prevHum;
int32_t prevPres;
float prevLightValue;

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    //init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  
  initTPH();
  initAirQuality();
  initLightSensor();
}

void loop() 
{
  processTPH();
  processAirQuality();
  processLightSensor();
  delay(3000);
}

void initLightSensor()
{
  pinMode(LightSensorPin,INPUT);
  float sensorValue = analogRead(LightSensorPin);
  float prevLightValue = (float)(1023 - sensorValue) * 10 / sensorValue;
  Device.Send(prevLightValue, LIGHT_SENSOR);
}

void processLightSensor()
{
  float sensorValue = analogRead(LightSensorPin);
  sensorValue = (float)(1023 - sensorValue) * 10 / sensorValue;
  if(sensorValue != prevLightValue)
  {
	Device.Send(sensorValue, LIGHT_SENSOR);
	prevLightValue = sensorValue;
  }
}

void initTPH()
{
  tph.begin();
  
  float prevTemp = tph.readTemperature();
  float prevHum = tph.readHumidity();
  int32_t prevPres = tph.readPressure();
  
  Serial.print("temp: ");
  Serial.print(prevTemp);
  
  Serial.print(", humidity: ");
  Serial.print(prevHum);
  
  Serial.print(", pressure: ");
  Serial.print(prevPres);
  Serial.println();

  Device.Send(prevTemp, TEMPERATURE_SENSOR);
  Device.Send(prevHum, HUMIDITY_SENSOR);
  Device.Send((short)prevPres, PRESURE_SENSOR);
}

void initAirQuality()
{
  airqualitysensor.init(AirQualityPin);
  
  prevAirQuality = airqualitysensor.getRawData();               //get the initial values so that we only send out values upon startup and when values have changed.
  Device.Send(prevAirQuality, AIR_QUALITY_SENSOR);
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

  if(temp != prevTemp)
  {
    Device.Send(temp, TEMPERATURE_SENSOR);
	prevTemp = temp;
  }
  if(hum != prevHum)
  {
    Device.Send(hum, HUMIDITY_SENSOR);
	prevHum = hum;
  }
  if(pres != prevPres)
  {
    Device.Send((short)pres, PRESURE_SENSOR);
	prevPres = pres;
  }
}

void processAirQuality()
{
    int value = airqualitysensor.getRawData();
    Device.Send(value, AIR_QUALITY_SENSOR);
}

void serialEvent1()
{
  Device.Process();                                                     //for future extensions -> actuators
}


