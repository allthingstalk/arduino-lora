/*
AllThingsTalk - SmartLiving.io LoRa Arduino library 
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#ifndef ATTDevice_h
#define ATTDevice_h

#include "Arduino.h"
#include <string.h>

#include "LoRaModem.h"
#include "LoraPacket.h"

#define BUTTON 1

//this class represents the ATT cloud platform.
class ATTDevice
{
	public:
		//create the object
		ATTDevice(LoRaModem* modem);
		
		/*connect with the http server (call first)
		returns: true when subscribe was successful, otherwise false.*/
		bool Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey);
		
		//create or update the specified asset. (call after connecting)
		//note: after this call, the name will be in lower case, so that it can be used to compare with the topic of incomming messages.
		//void AddAsset(short id, String name, String description, bool isActuator, String type);
		
		//send a bool data value to the cloud server for the sensor with the specified id.
		void Send(bool value, short id);
		
		//send an integer value to the cloud server for the sensor with the specified id.
		void Send(short value, short id);
		
		//send a string data value to the cloud server for the sensor with the specified id.
		void Send(String value, short id);
		
		//send a gloat data value to the cloud server for the sensor with the specified id.
		void Send(float value, short id);
		
		//sends the previously built complex data packet to the cloud for the sensor with the specified
		void Send(short id);
		
		//loads a bool data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void SendPartial(bool value);
		
		//loads a bool data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void SendPartial(short value);
		
		//loads a string data value into the data packet that is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void SendPartial(String value);
		
		//loads a float data value into the data packet tha is being prepared to send to the
		//cloud server.
		//the packet is sent after calling Send(id_of_sensor)
		void SendPartial(float value);
	
		//check for any new mqtt messages.
		void Process();
		
	private:	
		//builds the content that has to be sent to the cloud using mqtt (either a csv value or a json string)
		LoraPacket _data;
		LoRaModem* _modem;
};

#endif