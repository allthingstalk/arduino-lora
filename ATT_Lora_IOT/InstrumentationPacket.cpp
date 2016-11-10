/*
   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/  

#include <InstrumentationPacket.h>

//create the object
InstrumentationPacket::InstrumentationPacket()
{
	SetId(0x11);
}

unsigned char InstrumentationPacket::Write(unsigned char* result)
{
	unsigned char curPos =  LoRaPacket::Write(result);
	
	memcpy(result + curPos, _data, INST_DATA_SiZE);
	curPos += INST_DATA_SiZE;
	
	short packetLen = curPos - 3;												//write the packet length
	memcpy(result + 1, static_cast<const char*>(static_cast<const void*>(&packetLen)), sizeof(short));
	
	result[curPos] = calculateCheckSum(result + 3, curPos - 3);					//add the checksum
	curPos++;
	return curPos;
}

//resets the content of the packet back to 0 ->> all data will be removed
void InstrumentationPacket::Reset()
{
	memset(_data, NULL, INST_DATA_SiZE);
	SetId(0x11);
}

unsigned char InstrumentationPacket::getFrameType()
{
	return 0x60;									//the default packet type
}

bool InstrumentationPacket::SetParam(instrumentationParam param, int value)
{
	switch(param){
		case MODEM:
			_data[0] = (value << 4) | (_data[0] & 0x0F);     //make certain that any prev value is removed with 'and', add bits with or 
			return true;
		case FREQUENCYBAND:
			_data[0] = (value << 3) | (_data[0] & 0xF7);
			return true;
		case SP_FACTOR:
			_data[0] = value | (_data[0] & 0xF8);
			return true;
		case ADR:
			_data[1] = (value << 7) | (_data[1] & 0x7F);
			return true;
		case POWER_INDEX:
			_data[1] = (value << 4) | (_data[1] & 0x8F);
			return true;
		case BANDWIDTH:
			_data[1] = (value << 2) | (_data[1] & 0xF3);
			return true;
		case CODING_RATE:
			_data[1] = value | (_data[1] & 0xFC);
			return true;
		case DUTY_CYCLE:
			*((unsigned short*)(_data + 2)) = (unsigned short)value;
			return true;
		case SNR:
			_data[4] = value;
			return true;
		case GATEWAY_COUNT:
			_data[5] = value;
			return true;
		case RETRANSMISSION_COUNT:
			_data[6] = value;
			return true;
		case DATA_RATE:
			_data[7] = value;
			return true;
		default: return false;
	}
			
}