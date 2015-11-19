/*
AllThingsTalk - SmartLiving.io LoRa Arduino experiments
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#include <Wire.h>
#include "ATT_LoRa_IOT.h"
#include "keys.h"
#include "MicrochipLoRaModem.h"

#define SERIAL_BAUD 57600

int pushButton = 20;          
int doorSensor = 4;
MicrochipLoRaModem Modem(&Serial1);
ATTDevice Device(&Modem);

#define SEND_MAX_EVERY 30000                                //the mimimum time between 2 consecutive updates of visit counts that are sent to the cloud (can be longer, if the value hasn't changed)

bool prevButtonState;
bool prevDoorSensor;
short visitCount = 0;                                       //keeps track of the nr of visitors         
short prevVisitCountSent = 0;                               //we only send visit count max every 30 seconds, but only if the value has changed, so keep track of the value that was last sent to the cloud.
unsigned long lastSentAt = 0;                               //the time when the last visitcount was sent to the cloud.
bool isConnected = false;                                   //keeps track of the connection state.

void setup() 
{
  pinMode(pushButton, INPUT);                               // initialize the digital pin as an input.          
  pinMode(doorSensor, INPUT);
  
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());                //init the baud rate of the serial connection so that it's ok for the modem
  
  prevButtonState = digitalRead(pushButton);                //set the initial state
  prevDoorSensor = digitalRead(doorSensor);                 //set the initial state
  
  Device.SetMaxSendRetry(5);                                //for this use case we don't want to get stuck too long in sending data, we primarely want to capture visits and count them, sending the count can be done later on.
}

void tryConnect()
{
  isConnected = Device.Connect(DEV_ADDR, APPSKEY, NWKSKEY);
  if(isConnected == true)
  {
     Serial.println("Ready to send data");
     //todo: improvement: retrieved the count stored on disk so that it is not lost after power down
     sendVisitCount();                                  //always send the value at initial connection, keep the platform in sync with the latest change on the device.
  } 
  else
     Serial.println("connection will by retried later");  
}


void loop() 
{
  if(isConnected == false)                                          //if we previously failed to connect to the cloud, try again now.  This technique allows us to already collect visits before actually having established the connection.
    tryConnect();
  processButton();
  processDoorSensor();
  delay(100);
  if(isConnected && prevVisitCountSent != visitCount && lastSentAt + SEND_MAX_EVERY <= millis())
    sendVisitCount();
}

void sendVisitCount()
{
  Serial.print("send visit count: "); Serial.println(visitCount);
  Device.Send(visitCount, INTEGER_SENSOR);                       //we also send over the visit count so that the cloud is always in sync with the device (connection could have been established after the counter was changed since last connection).
  prevVisitCountSent = visitCount;
  lastSentAt = millis();
}

// check the state of the door sensor
void processDoorSensor()
{
  bool sensorRead = digitalRead(doorSensor);                         
  if(prevDoorSensor != sensorRead)
  {
    prevDoorSensor = sensorRead;
    if(sensorRead == true)                                          //door was closed, so increment the counter 
    {
        Serial.println("door closed");
        visitCount++;                                           //the door was opened and closed again, so increment the counter
    }
    else
        Serial.println("door open");
  }
}

void processButton()
{
  bool sensorRead = digitalRead(pushButton);                        // check the state of the button
  if (prevButtonState != sensorRead)                                // verify if value has changed
  {
     prevButtonState = sensorRead;
     if(sensorRead == true)                                         
     {
        Serial.println("button pressed");
        visitCount = 0;
     }
     else
        Serial.println("button released");
  }
}

void SendValue(bool val)
{
  Serial.println(val);
  
}


void serialEvent1()
{
  Device.Process();                                                     //for future extensions -> actuators
}


