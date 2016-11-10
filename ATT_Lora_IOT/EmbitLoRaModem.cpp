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

#include "EmbitLoRaModem.h"
#include <arduino.h>  			//required for the millis() function
#include "Utils.h"

#define PORT 0x01
#define SERIAL_BAUD 9600

#define PACKET_TIME_OUT 45000

unsigned char CMD_STOP[1] = { 0x30 };
unsigned char CMD_LORA_PRVNET[2] = { 0x25, 0xA0 };
unsigned char CMD_LORA_PRVNET_NO_ADR[2] = { 0x25, 0x80 };
unsigned char CMD_DEV_ADDR[1] = { 0x21 };
unsigned char CMD_APPSKEY[2] = { 0x26, 0x11};
unsigned char CMD_NWKSKEY[2] = { 0x26, 0x10};
unsigned char CMD_START[1] = { 0x31 };
unsigned char CMD_SEND_PREFIX[4] = { 0x50, 0x0C, 0x00, PORT }; 
unsigned char CMD_SEND_PREFIX_NO_ACK[4] = { 0x50, 0x08, 0x00, PORT }; 


unsigned char sendBuffer[52];

EmbitLoRaModem::EmbitLoRaModem(Stream* stream, Stream* monitor)
{
	_stream = stream;
	_monitor = monitor;
}

unsigned int EmbitLoRaModem::getDefaultBaudRate() 
{ 
	return SERIAL_BAUD; 
};

bool EmbitLoRaModem::Stop()
{
	PRINTLN("Sending the network stop command");
	SendPacket(CMD_STOP, sizeof(CMD_STOP));
	return ReadPacket();
}

bool EmbitLoRaModem::SetLoRaWan(bool adr)
{
	PRINTLN("Setting the network preferences to LoRaWAN private network");
	if(adr == true)
		SendPacket(CMD_LORA_PRVNET, sizeof(CMD_LORA_PRVNET));
	else
		SendPacket(CMD_LORA_PRVNET, sizeof(CMD_LORA_PRVNET_NO_ADR));
	return ReadPacket();
}

bool EmbitLoRaModem::SetDevAddress(unsigned char* devAddress)
{
	PRINTLN("Setting the DevAddr");
	SendPacket(CMD_DEV_ADDR, sizeof(CMD_DEV_ADDR), devAddress, 4); 
	return ReadPacket();
}

bool EmbitLoRaModem::SetAppKey(unsigned char* appKey)
{
	PRINTLN("Setting the AppSKey");   
	SendPacket(CMD_APPSKEY, sizeof(CMD_APPSKEY), appKey, 16);
	return ReadPacket();
}

bool EmbitLoRaModem::SetNWKSKey(unsigned char*  nwksKey)
{
	PRINTLN("Setting the NwkSKey");
	SendPacket(CMD_NWKSKEY, sizeof(CMD_NWKSKEY), nwksKey, 16);
	return ReadPacket();
}

bool EmbitLoRaModem::Start()
{
	PRINTLN("Sending the netowrk start command");
	SendPacket(CMD_START, sizeof(CMD_START));
	ReadPacket();
	
	//toddo: check result of readPacket and return actual success or not.
	return true;
}

bool EmbitLoRaModem::Send(LoRaPacket* packet, bool ack)
{
	unsigned char length = packet->Write(sendBuffer);
	PRINTLN("Sending payload: ");
	for (unsigned char i = 0; i < length; i++) {
		printHex(sendBuffer[i]);
	}
	PRINTLN();
  
	if(ack == true)
		SendPacket(CMD_SEND_PREFIX, sizeof(CMD_SEND_PREFIX), sendBuffer, length);
	else
		SendPacket(CMD_SEND_PREFIX_NO_ACK, sizeof(CMD_SEND_PREFIX_NO_ACK), sendBuffer, length);
	unsigned char result = ReadPacket(3);
	if(result != 0){
		PRINTLN("Failed to send packet");
		return false;
	}
	else
		return true;
	
}

void EmbitLoRaModem::SendPacket(unsigned char* data, uint16_t length)
{
	PRINT("Sending: ");
	uint16_t packetLength = length + 3;
	unsigned char* len = (unsigned char*)&packetLength;
	unsigned char CRC = len[1] + len[0];

	  //Little Endian vs big endian
	sendByte(len[1]);
	sendByte(len[0]);

	for (size_t i = 0; i < length; i++) {
		CRC += data[i];
		sendByte(data[i]);
	}
	  
	sendByte(CRC);
	PRINTLN();
}

void EmbitLoRaModem::SendPacket(unsigned char* data, uint16_t length, unsigned char* data2, uint16_t length2)
{
	PRINT("Sending: ");
	uint16_t packetLength = length + length2 + 3;
	unsigned char* len = (unsigned char*)&packetLength;
	unsigned char CRC = len[1] + len[0];

	  //Little Endian vs big endian
	sendByte(len[1]);
	sendByte(len[0]);

	for (size_t i = 0; i < length; i++) {
		CRC += data[i];
		sendByte(data[i]);
	}
	
	for (size_t i = 0; i < length2; i++) {
		CRC += data2[i];
		sendByte(data2[i]);
	}
	  
	sendByte(CRC);
	PRINTLN();
}

void EmbitLoRaModem::sendByte(unsigned char data)
{
	_stream->write(data);
	printHex(data);
}

bool EmbitLoRaModem::ReadPacket()
{
	uint32_t maxTS = millis() + PACKET_TIME_OUT;
	uint16_t length = 4;
	unsigned char firstByte = 0;
  
	PRINT("Receiving: "); 

	size_t i = 0;
	while ((maxTS > millis()) && (i < length)) {
		while ((!_stream->available()) && (maxTS > millis()));

		if (_stream->available()) {
			unsigned char value = _stream->read();
			if (i == 0) {
				firstByte = value;
			} else if (i == 1) {
				length = firstByte * 256 + value;
			}
			printHex(value);
			i++;
		}
	}

	if (i < length) {
		PRINT("Timeout");
		return false;
	}
	PRINTLN();
	return true;
}

unsigned char EmbitLoRaModem::ReadPacket(unsigned char index)
{
	uint32_t maxTS = millis() + PACKET_TIME_OUT;
	uint16_t length = 4;
	unsigned char firstByte = 0;
	unsigned char result = 0;
  
	PRINT("Receiving: "); 

	size_t i = 0;
	while ((maxTS > millis()) && (i < length)) {
		while ((!_stream->available()) && (maxTS > millis()));

		if (_stream->available()) {
			unsigned char value = _stream->read();
			if (i == 0)
				firstByte = value;
			else if (i == 1)
				length = firstByte * 256 + value;
			else if(i == index)
			   result = value;
			printHex(value);
			i++;
		}
	}

	if (i < length) {
		PRINT("Timeout");
	}
	PRINTLN();
	return result;
}

//process any incoming packets from the modem
 void EmbitLoRaModem::ProcessIncoming()
 {
	ReadPacket();
 }

void EmbitLoRaModem::printHex(unsigned char hex)
{
  char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  PRINT(hexTable[hex /16]);
  PRINT(hexTable[hex % 16]);
  PRINT(' ');
}
//extract the specified instrumentation parameter from the modem and return the value
int EmbitLoRaModem::GetParam(instrumentationParam param)
{
	PRINTLN("to be implemented: GetParam for embit modems");
}
//returns the id number of the modem type. See the container definition for the instrumentation container to see more details.
int EmbitLoRaModem::GetModemId()
{
	return 2;
}
