/*
	LoraPacket.cpp - SmartLiving.io Arduino library 
*/


#include "LoraPacket.h"

//create the object
LoraPacket::LoraPacket()
{
}

unsigned char LoraPacket::Write(unsigned char* result)
{
	unsigned char curPos = 0;
	
	result[0] = 0x7E;
	result[3] = 0x40;
	result[4] = contId;
	result[5] = boolValues;
	curPos = 6;
	if(nrInts > 0)
	{
		result[curPos++] = nrInts;
		for(int i = 0; i < nrInts; i++)
		{
			short len = sizeof intValues[i];
			memcpy(result + curPos, &intValues[i], len);
			curPos += len;
		}
	}
	if(nrFloats > 0)
	{
		result[curPos++] = nrFloats;
		for(int i = 0; i < nrFloats; i++)
		{
			short len = sizeof floatValues[i];
			memcpy(result + curPos, &floatValues[i], len);
			curPos += len;
		}
	}
	if(stringPos > 0)
	{	
		memcpy(result + curPos, stringValues, stringPos);
		curPos += stringPos;
	}
	
	//write the packet length
	short packetLen = curPos - 3;
	memcpy(result + 1, static_cast<const char*>(static_cast<const void*>(&packetLen)), sizeof(short));
	
	//add the checksum
	result[curPos] = calculateCheckSum(result + 3, curPos - 3);
	curPos++;
	return curPos;
}

//assigns the asset/container id to the packet
void LoraPacket::SetId(unsigned char id)
{
	contId = id;
}

unsigned char LoraPacket::calculateCheckSum(unsigned char* toSend, short len)
{
	int sum = 0;
	for(int i = 0; i < len; i++)
		sum += toSend[i];
		
	while(sum > 0xFF)
	{
		toSend = (unsigned char*)&sum;
		int newsum = 0;
		len = 4;
		for(int i = 0; i < len; i++)
			newsum += toSend[i];
		sum = newsum;
	}
	
	return 0xFF - (unsigned char)sum;
}


bool LoraPacket::Add(bool value)
{
	if(nrBools >= 8)
		return false;
		//throw Exception("too many bool values in packet");
	unsigned char val = value == true ? 1:0;
	
	val = val << nrBools;
	nrBools++;
	boolValues |= val;
	return true;
}

bool LoraPacket::Add(short value)
{
	if(nrInts >= 16)
		return false;
		//throw Exception("too many int values in packet");
	intValues[nrInts++] = value;
	return true;
}

bool LoraPacket::Add(String value)
{
	int len = value.length();
	if(stringPos + len >= 48)
		return false;
		//throw Exception("string too long for packet");
	value.toCharArray(stringValues + stringPos, len);
	stringPos += len;
	return true;
}

bool LoraPacket::Add(float value)
{
	if(nrFloats >= 16)
		return false;
		//throw Exception("too many float values in packet");
	floatValues[nrFloats++] = value;
	return true;
}

void LoraPacket::Reset()
{
	stringPos = 0;
	nrFloats = 0;
	nrInts = 0;
	nrBools = 0;
	boolValues = 0;
}
