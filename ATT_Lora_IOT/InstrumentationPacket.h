/*
AllThingsTalk - SmartLiving.io Arduino library 
Released into the public domain.

Original author: Jan Bogaerts (2015-2016)
*/

#ifndef LoRaPacket_h
#define LoRaPacket_h

#include <string.h>
#include <Stream.h>

typedef enum {DATA_RATE, FREQUENCYBAND, CHANNEL, POWER_INDEX, ADR, DUTY_CYCLE, GATEWAY_COUNT, SNR, SP_FACTOR} instrumentationParam;

//this class represents the ATT cloud platform.
class InstrumentationPacket: public LoRaPacket
{
	public:
		//create the object
		InstrumentationPacket();
		
		//writes the packet content to the specified byte array. This must be at least 51 bytes long.
		//returns: the nr of bytes actually written to the array.
		virtual unsigned char Write(unsigned char* result);
		
		
		//resets the content of the packet back to 0 ->> all data will be removed
		void Reset();
		
		void SetParam(instrumentationParam param, int value);
	private:	
		unsigned char _data[10];
};

#endif