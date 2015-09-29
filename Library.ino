//You will need to install the following libraries:
//Sodaq_BMP085
//Sodaq_SHT2x
//For information on installing libraries please refer to:
//www.arduino.cc/en/Guide/Libraries

#include <Wire.h>
#include <Sodaq_SHT2x.h>
#include "ATT_Lora_IOT.h"
#include "EmbitLoRaModem.h"

#define SERIAL_BAUD 57600
#define SERIAL1_BAUD 9600


//The last 4 bytes are the DevAddr, leave the first byte unchanged
uint8_t DEV_ADDR[4] = { 0x00, 0x00, 0x13, 0x00 };

//The last 16 bytes are the key values, leave the first two bytes unchanged
uint8_t APPSKEY[16] = { 0xef, 0xb6, 0x1c, 0x43, 0x3a, 0xa2, 0x8a, 0x9f, 0xb3, 0x46, 0xc8 , 0xf9, 0x2b, 0x6b , 0x47, 0x2a };

uint8_t NWKSKEY[16] = { 0x73, 0x63, 0x98, 0x79, 0x3b, 0x0a, 0xd9, 0xce, 0x54, 0x91, 0xda, 0xe9, 0xc3, 0xdc, 0xd3, 0x85 };

int DigitalSensor = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int Sensor2 = 4;                                        // Digital Sensor is connected to pin D8 on grove shield 
EmbitLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

void setup() 
{
  pinMode(DigitalSensor, INPUT);					            // initialize the digital pin as an input.          
  pinMode(Sensor2, INPUT);					            // initialize the digital pin as an input.          
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(SERIAL1_BAUD);
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
}

bool sensorVal = false;							        //only send every x amount of time.

void loop() 
{
  bool sensorRead = digitalRead(DigitalSensor);			        // read status Digital Sensor
  //bool sensorRead2 = digitalRead(Sensor2);			        // read status Digital Sensor 
  bool res = sensorRead; //!(sensorRead | sensorRead2);
  //Serial.println(res);
  if (sensorVal != res) 				                // verify if value has changed
  {
    sensorVal = res;
    if (res >= 1)
      Device.Send(true, BUTTON);
    else
      Device.Send(false, BUTTON);
  }
  delay(100);
}



void serialEvent1()
{
  Device.Process();
}


