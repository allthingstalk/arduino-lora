/*
AllThingsTalk -  LoRa Arduino library 

   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

Original author: Jan Bogaerts (2015)
*/

#ifndef ATTDevice_h
#define ATTDevice_h

//#include "Arduino.h"
#include <string.h>
#include <Stream.h>

#include <LoRaModem.h>
#include <DataPacket.h>
#include <instrumentationParamEnum.h>
#include <InstrumentationPacket.h>

/////////////////////////////////////////////////////////////
//	Configuration
/////////////////////////////////////////////////////////////
#define SEND_MAX_RETRY 30			//the default max nr of times that 'send' functions will retry to send the same value.
#define MIN_TIME_BETWEEN_SEND 0 //the minimum time between 2 consecutive calls to Send data.

#define VERSION "1.1"


/////////////////////////////////////////////////////////////

#define BINARY_SENSOR ((short)1)
#define BINARY_TILT_SENSOR ((short)2)
#define PUSH_BUTTON ((short)3)
#define DOOR_SENSOR ((short)4)
#define TEMPERATURE_SENSOR ((short)5)
#define LIGHT_SENSOR ((short)6)
#define PIR_SENSOR ((short)7)
#define ACCELEROMETER ((short)8)
#define GPS ((short)9)
#define PRESSURE_SENSOR ((short)10)
#define HUMIDITY_SENSOR ((short)11)
#define LOUDNESS_SENSOR ((short)12)
#define AIR_QUALITY_SENSOR ((short)13)
#define BATTERY_LEVEL ((short)14)
#define INTEGER_SENSOR ((short)15)
#define NUMBER_SENSOR ((short)16)

//this class represents the ATT cloud platform.
class ATTDevice
{
	public:
		//create the object
		//modem: the object that respresents the modem that should be used.
		//monitor: the stream used to write log lines to.
		ATTDevice(LoRaModem* modem, Stream* monitor = NULL);
		
		/*connect with the http server (call first)
		returns: true when subscribe was successful, otherwise false.*/
		bool Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr = true);
		
		//create or update the specified asset. (call after connecting)
		//note: after this call, the name will be in lower case, so that it can be used to compare with the topic of incomming messages.
		//void AddAsset(short id, String name, String description, bool isActuator, String type);
		
		//send a bool data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknolodge, otherwise no acknowledge is waited for.
		bool Send(bool value, short id, bool ack = true);
		
		//send an integer value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool Send(short value, short id, bool ack = true);
		
		//send a string data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool Send(String value, short id, bool ack = true);
		
		//send a gloat data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool Send(float value, short id, bool ack = true);
		
		//sends the previously built complex data packet to the cloud for the sensor with the specified
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool Send(short id, bool ack = true);
		
		//sends the previously built Lora packet to the cloud. This can be a regulat packet, instrumentation packet,...
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool Send(LoRaPacket* data, bool ack = true);
		
		//collects all the instrumentation data from the modem (RSSI, ADR, datarate,..) and sends it over
		//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
		bool SendInstrumentation(bool ack = true);
		
		//loads a bool data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void Queue(bool value);
		
		//loads a bool data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void Queue(short value);
		
		//loads a string data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void Queue(String value);
		
		//loads a float data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void Queue(float value);
	
		//check for any new mqtt messages.
		void Process();
		
		
		//set the minimum amount of time between 2 consecutive messages that are sent to the cloud.
		//default value: 15 seconds.
		//minTimeBetweenSend: the nr of milli seconds that should be between 2 data packets.
		void SetMinTimeBetweenSend(short minTimeBetweenSend) { _minTimeBetweenSend = minTimeBetweenSend; };
		
	private:	
		Stream *_monitor;
		//builds the content that has to be sent to the cloud using mqtt (either a csv value or a json string)
		DataPacket _data;
		LoRaModem* _modem;
		short _minTimeBetweenSend;
		unsigned long _lastTimeSent;					//the last time that a message was sent, so we can block sending if user calls send to quickly
		
		//store the param in the  data packet, and print to serial.
		void SetInstrumentationParam(InstrumentationPacket* data, instrumentationParam param, char* name, int value);
		
};

#endif
