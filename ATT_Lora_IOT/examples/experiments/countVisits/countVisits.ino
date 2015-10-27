
#include <Wire.h>
#include "ATT_Lora_IOT.h"
#include "keys.h"
//#include "EmbitLoRaModem.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600

int pushButton = 20;                                        // Digital Sensor is connected to pin D8 on grove shield 
int doorSensor = 4;
//EmbitLoRaModem Modem(&Serial1);
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

bool prevButtonState;
bool prevDoorSensor;

void setup() 
{
  pinMode(pushButton, INPUT);					            // initialize the digital pin as an input.          
  pinMode(doorSensor, INPUT);
  
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					//init the baud rate of the serial connection so that it's ok for the modem
  
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  prevButtonState = digitalRead(pushButton);						//set the initial state
  Device.Send(val, PUSH_BUTTON);
  prevDoorSensor = digitalRead(doorSensor);						//set the initial state
  Device.Send(0, DOOR_SENSOR);
}

void loop() 
{
  bool sensorRead = digitalRead(pushButton);			        // check the state of the button
  if (prevButtonState != sensorRead) 				                // verify if value has changed
  {
     prevButtonState = sensorRead;
     Device.Send(val, PUSH_BUTTON);
	 if(sensorVal == true)											//incrementing the counter is done on the cloud, cause it knows the previous count.
		Serial.println("button pressed");
	else
		Serial.println("button released");
  }
  sensorRead = digitalRead(doorSensor);			        			// check the state of the door sensor
  if(prevDoorSensor != sensorRead)
  {
	prevDoorSensor = sensorRead;
	Device.Send(val, DOOR_SENSOR);
	if(sensorVal == true)				
		Serial.println("door closed");
	else
		Serial.println("door open");
  }
  delay(100);
}

void SendValue(bool val)
{
  Serial.println(val);
  
}


void serialEvent1()
{
  Device.Process();														//for future extensions -> actuators
}


