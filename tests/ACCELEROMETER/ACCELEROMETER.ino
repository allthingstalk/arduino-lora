//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include "MMA7660.h"
#include <Sodaq_SHT2x.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
#include "EmbitLoRaModem.h"
//#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600



int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int ActionLed = 4;                                        	   // activated when the modem is sending a datapacket
EmbitLoRaModem Modem(&Serial1);
//MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);



void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  pinMode(ActionLed, OUTPUT);					           			// initialize the digital pin as an output -> show that modem is sending.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  pinMode(LED1, OUTPUT);									//indicate that the device is running -> still battery power left
  digitalWrite(LED1, HIGH);
}

float x = 1.1;
float y = 2.2;
float z = 3.3;

void loop() 
{
  SendValue();
  x = x + 10.01;
  y = y + 10.01;
  z = z + 10.01;
  delay(300);
}

void SendValue()
{
  digitalWrite(ActionLed, 1);
  Device.Queue(x);
  Device.Queue(y);
  Device.Queue(z);
  Device.Send(ACCELEROMETER);
  digitalWrite(ActionLed, 0);
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


