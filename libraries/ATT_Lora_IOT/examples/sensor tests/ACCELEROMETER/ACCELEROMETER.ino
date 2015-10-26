//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "MMA7660.h"
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600


MMA7660 accelemeter;
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  accelemeter.init(); 
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

float x;
float y;
float z;

void loop() 
{
  accelemeter.getAcceleration(&x,&y,&z);
  Serial.println("accleration of X/Y/Z: ");
	Serial.print(x);
	Serial.println(" g");
	Serial.print(y);
	Serial.println(" g");
	Serial.print(z);
	Serial.println(" g");
	Serial.println("*************");
  //SendValue();
  
  delay(1000);
}

void SendValue()
{
  Device.Queue(x);
  Device.Queue(y);
  Device.Queue(z);
  Device.Send(ACCELEROMETER);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


