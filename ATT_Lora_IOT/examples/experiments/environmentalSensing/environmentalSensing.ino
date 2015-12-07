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
 *
 * EXPLENATION:
 * We will measure our environment using 6 sensors. Approximately, every 2 minutes, all values 
 * will be read and sent to the SmartLiving Developer Cloud.
 */

#include <Wire.h>
#include <Sodaq_TPH.h>
#include "AirQuality2.h"
#include <ATT_LoRa_IOT.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>


#define SERIAL_BAUD 57600
#define AirQualityPin A0
#define LightSensorPin A2
#define SoundSensorPin A4

#define SEND_EVERY 20000


MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;

float soundValue;
float lightValue;
float temp;
float hum;
float pres;
short airValue;

void setup() 
{
  pinMode(GROVEPWR, OUTPUT);                                    //turn on the power for the secondary row of grove connectors.
  digitalWrite(GROVEPWR, HIGH);

  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    //init the baud rate of the serial connection so that it's ok for the modem
  while(!Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY))            //there is no point to continue if we can't connect to the cloud: this device's main purpose is to send data to the cloud.
  {
    Serial.println("retrying...");
    delay(200);
  }
  Device.SetMaxSendRetry(1);                                    //just try to send data 1 time, if it fails, just go to the next data item.  if the data must arrive in the cloud before continuing, use -1, to indicate that data has to be successfully sent before moving on to the next item
  Device.SetMinTimeBetweenSend(15000);                          //wait between sending 2 messages, to make certain that the base station doesn't punish us for sending too much data too quickly.
  initSensors();
}

void loop() 
{
  ReadSensors();
  DisplaySensors();
  SendSensors();
  Serial.print("delay for: ");
  Serial.println(SEND_EVERY);
  delay(SEND_EVERY);
}

void initSensors()
{
    Serial.println("initializing sensors, this can take a few seconds...");
    pinMode(SoundSensorPin,INPUT);
    pinMode(LightSensorPin,INPUT);
    tph.begin();
    airqualitysensor.init(AirQualityPin);
}

void ReadSensors()
{
    Serial.println("Start sampling sensors. ");
    Serial.println("----------------------- ");
    soundValue = analogRead(SoundSensorPin);
    lightValue = analogRead(LightSensorPin);
    lightValue = lightValue * 3.3 / 1023;                           //convert to lux, this is based on the voltage that the sensor receives
    lightValue = pow(10, lightValue);
    
    temp = tph.readTemperature();
    hum = tph.readHumidity();
    pres = tph.readPressure();
    
    airValue = airqualitysensor.getRawData();
}

void SendSensors()
{
    Serial.println("Start uploading data to the ATT cloud Platform. ");
  Serial.println("-----------------------------------------------");
    Serial.println("sending sound value... ");
    Device.Send(soundValue, LOUDNESS_SENSOR);
    Serial.println("sending light value... "); 
    Device.Send(lightValue, LIGHT_SENSOR);
    Serial.println("sending temperature value... ");
    Device.Send(temp, TEMPERATURE_SENSOR);
    Serial.println("sending humidity value... ");  
    Device.Send(hum, HUMIDITY_SENSOR);
    Serial.println("sending pressure value... ");  
    Device.Send(pres, PRESSURE_SENSOR);
    Serial.println("sending air quality value... ");  
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


