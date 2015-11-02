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


/////////////////////////////////////////////////////////////
//	Configuration
/////////////////////////////////////////////////////////////
#define SEND_MAX_RETRY 30			//the default max nr of times that 'send' functions will retry to send the same value.
#define MIN_TIME_BETWEEN_SEND 15000 //the minimum time between 2 consecutive calls to Send data.




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
		ATTDevice(LoRaModem* modem);
		
		/*connect with the http server (call first)
		returns: true when subscribe was successful, otherwise false.*/
		bool Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr = true);
		
		//create or update the specified asset. (call after connecting)
		//note: after this call, the name will be in lower case, so that it can be used to compare with the topic of incomming messages.
		//void AddAsset(short id, String name, String description, bool isActuator, String type);
		
		//send a bool data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknolodge, otherwise no acknolodge is waited for.
		void Send(bool value, short id, bool ack = true);
		
		//send an integer value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknolodge, otherwise no acknolodge is waited for.
		void Send(short value, short id, bool ack = true);
		
		//send a string data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknolodge, otherwise no acknolodge is waited for.
		void Send(String value, short id, bool ack = true);
		
		//send a gloat data value to the cloud server for the sensor with the specified id.
		//if ack = true -> request acknolodge, otherwise no acknolodge is waited for.
		void Send(float value, short id, bool ack = true);
		
		//sends the previously built complex data packet to the cloud for the sensor with the specified
		//if ack = true -> request acknolodge, otherwise no acknolodge is waited for.
		void Send(short id, bool ack = true);
		
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
		
		//set the max nr of times that the 'Send' functions will try to resend a message when previously not successful.
		//default value = 30
		//set to -1 for continuous until success.
		void SetMaxSendRetry(short maxRetries) { _maxRetries = maxRetries; };
		
	private:	
		//builds the content that has to be sent to the cloud using mqtt (either a csv value or a json string)
		LoraPacket _data;
		LoRaModem* _modem;
		short _maxRetries;								//the max nr of times that a send function will try to resend a message.
		unsigned long _lastTimeSent;					//the last time that a message was sent, so we can block sending if user calls send to quickly
		
};

#endif