/*
	LoRaPacket.cpp - SmartLiving.io Arduino library 
*/


#include "LoRaPacket.h"

//create the object
LoRaPacket::LoRaPacket()
{
}

unsigned char LoRaPacket::Write(unsigned char* result)
{
	result[0] = 0x7E;
	result[3] = getFrameType();
	result[4] = contId;
	return 5;
}

//assigns the asset/container id to the packet
void LoRaPacket::SetId(unsigned char id)
{
	contId = id;
}

unsigned char LoRaPacket::getFrameType()
{
	return 0x40;									//the default packet type
}

unsigned char LoRaPacket::calculateCheckSum(unsigned char* toSend, short len)
{
	int sum = 0;
	for(int i = 0; i < len; i++)
		sum += toSend[i];
		
	while(sum > 0xFF)
	{
		toSend = (unsigned char*)&sum;
		int newsum = 0;
		len = sizeof(int);
		for(int i = 0; i < len; i++)
			newsum += toSend[i];
		sum = newsum;
	}
	
	return 0xFF - (unsigned char)sum;
}
