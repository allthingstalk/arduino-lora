//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "MMA7660.h"
#include <Sodaq_SHT2x.h>
#include "ATT_Lora_IOT.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"
#include "keys.h"

#define SERIAL_BAUD 57600

int DigitalSensor = 20;                              // pin this sensor is connected to
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

bool sensorVal = false;

void setup() 
{
  pinMode(DigitalSensor, INPUT);                     // initialize the digital pin as an input
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());         // init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");

  sensorVal = digitalRead(DigitalSensor);
  SendValue(sensorVal);                              // send initial state
}

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);      // read status Digital Sensor
  if (sensorVal != sensorRead)                       // verify if value has changed
  {
    sensorVal = sensorRead;
    SendValue(sensorRead);
  }
  delay(100);
}

void SendValue(bool val)
{
  Serial.println(sensorVal);
  Device.Send(val, BINARY_SENSOR);
}

void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}
