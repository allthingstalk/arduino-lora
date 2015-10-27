//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "AirQuality2.h"
#include"Arduino.h"
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


#define AnalogSensor A4

//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);
AirQuality2 airqualitysensor;
int value = -1;

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  airqualitysensor.init(AnalogSensor);
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

//short value;

void loop() 
{
  value = airqualitysensor.getRawData();
  SendValue();
  delay(3000);
}

void SendValue()
{
  Serial.print("sending: ");
  Serial.println(value);
  //Device.Send(value, AIR_QUALITY_SENSOR);
}


void serialEvent1()
{
//  Device.Process();														//for future extensions -> actuators
}


