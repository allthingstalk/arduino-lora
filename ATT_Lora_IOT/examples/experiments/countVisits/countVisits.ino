/*
AllThingsTalk - SmartLiving.io LoRa Arduino experiments
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

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
short visitCount = 0;                                       //keeps track of the nr of visitors         
bool someoneInside = false;                                 //keeps track wether the door opened for letting someone in or out.

void setup() 
{
  pinMode(pushButton, INPUT);                               // initialize the digital pin as an input.          
  pinMode(doorSensor, INPUT);
  
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                    //init the baud rate of the serial connection so that it's ok for the modem
  
  Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  Serial.println("Ready to send data");
  
  prevButtonState = digitalRead(pushButton);                        //set the initial state
  Device.Send(prevButtonState, PUSH_BUTTON);
  prevDoorSensor = digitalRead(doorSensor);                     //set the initial state
  Device.Send(false, DOOR_SENSOR);                              //when the device is booted up, usually someone is inside to place/turn on the device, so the door is open = false
}

void loop() 
{
  bool sensorRead = digitalRead(pushButton);                    // check the state of the button
  if (prevButtonState != sensorRead)                                // verify if value has changed
  {
     prevButtonState = sensorRead;
     Device.Send(sensorRead, PUSH_BUTTON);
     if(sensorRead == true)                                          //incrementing the counter is done on the cloud, cause it knows the previous count.
     {
        Serial.println("button pressed");
        Device.Send((short)0, INTEGER_SENSOR);
		    visitCount = 0;
     }
     else
        Serial.println("button released");
  }
  sensorRead = digitalRead(doorSensor);                             // check the state of the door sensor
  if(prevDoorSensor != sensorRead)
  {
    prevDoorSensor = sensorRead;
    Device.Send(sensorRead, DOOR_SENSOR);
    if(sensorRead == true)                                       //door was closed, so increment the counter 
    {
        Serial.println("door closed");
        if(someoneInside == true)								//when a person enters, the door is opened & closed, when he leaves, the door is again opened & closed. We want to count the visits, not the nr of times that th door was opened & closed. This is of course an approximation.
        {
            someoneInside = false;
            visitCount++;                                           //the door was opened and closed again, so increment the counter
            Device.Send(visitCount, INTEGER_SENSOR);
        }
        else
            someoneInside = true;
    }
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
  Device.Process();                                                     //for future extensions -> actuators
}


