#include <Wire.h>
#include <Sodaq_TPH.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"


#define SERIAL_BAUD 57600


//EmbitLoRaModem Modem(&Serial1);
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
  int32_t pres = tph.readPressure();
  
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
  Device.Send((short)pres, PRESURE_SENSOR);
  
  delay(3000);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


