/*
AllThingsTalk Abstract class for LoRa modems

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

#ifndef LoRaModem_h
#define LoRaModem_h

#include <LoRaPacket.h>
#include <instrumentationParamEnum.h>

//this class represents the ATT cloud platform.
class LoRaModem
{
	public:
		// Returns the required baudrate for the device
		virtual unsigned int getDefaultBaudRate() = 0;
		//stop the modem.
		virtual bool Stop() = 0;
		//set the modem in LoRaWan mode (vs private networks)
		virtual bool SetLoRaWan(bool adr = true) = 0;
		//assign a device address to the modem
		//devAddress must be 4 bytes long
		virtual bool SetDevAddress(unsigned char* devAddress) = 0;
		//set the app session key for the modem communication
		//app session key must be 16 bytes long
		virtual bool SetAppKey(unsigned char* appKey) = 0;
		//set the network session key
		//network session key must be 16 bytes long
		virtual bool SetNWKSKey(unsigned char*  nwksKey) = 0;
		//start the modem: returns true if successful
		virtual bool Start() = 0;
		//send a data packet to the server
		//ack = true -> request ack
		virtual bool Send(LoRaPacket* packet, bool ack = true) = 0;
		//process any incoming packets from the modem
		virtual void ProcessIncoming() = 0;
		//extract the specified instrumentation parameter from the modem and return the value
		virtual int GetParam(instrumentationParam param) = 0;
		//returns the id number of the modem type. See the container definition for the instrumentation container to see more details.
		virtual int GetModemId() = 0;
};

#endif