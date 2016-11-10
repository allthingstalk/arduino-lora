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

#include "DataPacket.h"

//create the object
DataPacket::DataPacket()
{
}

unsigned char DataPacket::Write(unsigned char* result)
{
	unsigned char curPos =  LoRaPacket::Write(result);
	
	result[curPos] = boolValues;
	curPos++;
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
	else if(nrFloats > 0 || stringPos > 0)
		result[curPos++] = 0;
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
	else if(stringPos > 0)
		result[curPos++] = 0;
	if(stringPos > 0)
	{	
		memcpy(result + curPos, stringValues, stringPos);
		curPos += stringPos;
	}	
	short packetLen = curPos - 3;												//write the packet length
	memcpy(result + 1, static_cast<const char*>(static_cast<const void*>(&packetLen)), sizeof(short));
	
	result[curPos] = calculateCheckSum(result + 3, curPos - 3);					//add the checksum
	curPos++;
	return curPos;
}



bool DataPacket::Add(bool value)
{
	if(nrBools >= 8)
		return false;
	unsigned char val = value == true ? 1:0;
	
	val = val << nrBools;
	nrBools++;
	boolValues |= val;
	return true;
}

bool DataPacket::Add(short value)
{
	if(nrInts >= 16)
		return false;
	intValues[nrInts++] = value;
	return true;
}

bool DataPacket::Add(String value)
{
	int len = value.length();
	if(stringPos + len >= 48)
		return false;
	value.toCharArray(stringValues + stringPos, len);
	stringPos += len;
	return true;
}

bool DataPacket::Add(float value)
{
	if(nrFloats >= 16)
		return false;
	floatValues[nrFloats++] = value;
	return true;
}

void DataPacket::Reset()
{
	stringPos = 0;
	nrFloats = 0;
	nrInts = 0;
	nrBools = 0;
	boolValues = 0;
}
