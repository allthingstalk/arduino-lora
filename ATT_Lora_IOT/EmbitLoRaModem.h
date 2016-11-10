/*
AllThingsTalk - Communicate with Embit lora modems through binary AT commands

   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

Original author: Jan Bogaerts (2015)
*/

#ifndef EmbitLoRaModem_h
#define EmbitLoRaModem_h

#include <LoRaModem.h>
#include <LoRaPacket.h>
#include <instrumentationParamEnum.h>

//this class represents the ATT cloud platform.
class EmbitLoRaModem: public LoRaModem
{
	public:
		//create the object
		EmbitLoRaModem(Stream* stream, Stream* monitor = NULL);
		// Returns the required baudrate for the device
		unsigned int getDefaultBaudRate();
		//stop the modem.
		bool Stop();
		//set the modem in LoRaWan mode (vs private networks)
		//adr = adaptive data rate. true= use, false = none adaptive data rate
		bool SetLoRaWan(bool adr = true);
		//assign a device address to the modem
		//devAddress must be 4 bytes long
		bool SetDevAddress(unsigned char* devAddress);
		//set the app session key for the modem communication
		//app session key must be 16 bytes long
		bool SetAppKey(unsigned char* appKey);
		//set the network session key
		//network session key must be 16 bytes long
		bool SetNWKSKey(unsigned char*  nwksKey);
		//start the modem , returns true if successful
		bool Start();
		//send a data packet to the server
		bool Send(LoRaPacket* packet, bool ack = true);
		//process any incoming packets from the modem
		void ProcessIncoming();
		//extract the specified instrumentation parameter from the modem and return the value
		int GetParam(instrumentationParam param);
		//returns the id number of the modem type. See the container definition for the instrumentation container to see more details.
		int GetModemId();
	private:
		void printHex(unsigned char hex);
		void sendByte(unsigned char data);
		void SendPacket(unsigned char* data, uint16_t length);
		void SendPacket(unsigned char* data, uint16_t length, unsigned char* data2, uint16_t length2);
		bool ReadPacket();
		//reads a packet from the modem and returns the value of the byte at the specified index position
		unsigned char ReadPacket(unsigned char index);
		
		Stream *_monitor;
		Stream* _stream;					//the stream to communicate with the lora modem.
};

#endif