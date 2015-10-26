/*
AllThingsTalk - SmartLiving.io Arduino library 
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#ifndef LoraPacket_h
#define LoraPacket_h

#include "Arduino.h"
#include <string.h>
#include <Stream.h>

//this class represents the ATT cloud platform.
class LoraPacket
{
	public:
		//create the object
		LoraPacket();
		
		//writes the packet content to the specified byte array. This must be at least 51 bytes long.
		//returns: the nr of bytes actually written to the array.
		unsigned char Write(unsigned char* result);
		
		//assigns the asset/container id to the packet
		void SetId(unsigned char id);
		
		//loads a bool data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		//returns true if successful, otherwise false.
		bool Add(bool value);
		
		//loads a bool data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		//returns true if successful, otherwise false.
		bool Add(short value);
		
		//loads a bool data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		//returns true if successful, otherwise false.
		bool Add(String value);
		
		//loads a bool data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		//returns true if successful, otherwise false.
		bool Add(float value);
		
		//resets the content of the packet back to 0 ->> all data will be removed
		void Reset();
	private:	
		
		//builds the content that has to be sent to the cloud using mqtt (either a csv value or a json string)
		char stringValues[48];
		short intValues[16];
		float floatValues[16];
		unsigned char contId;
		unsigned char boolValues;
		unsigned char stringPos;
		unsigned char nrInts;
		unsigned char nrFloats;
		unsigned char nrBools;
		
		unsigned char calculateCheckSum(unsigned char* toSend, short len);
};

#endif