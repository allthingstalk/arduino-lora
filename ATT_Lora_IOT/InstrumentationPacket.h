/*
AllThingsTalk  Arduino library 

   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

Original author: Jan Bogaerts (2015-2016)
*/

#ifndef InstrumentationPacket_h
#define InstrumentationPacket_h

#include <string.h>
#include <Stream.h>

#include <instrumentationParamEnum.h>
#include <LoRaPacket.h>

#define INST_DATA_SiZE 8

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
		
		bool SetParam(instrumentationParam param, int value);
	protected:
		//returns the frame type number for this lora packet. The default value is 0x40. Inheritors that render other packet types can overwrite this.
		unsigned char getFrameType();
	private:	
		unsigned char _data[INST_DATA_SiZE];
};

#endif