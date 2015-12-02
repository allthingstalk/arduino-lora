/*
AllThingsTalk - SmartLiving.io Abstract class for LoRa modems
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#ifndef LoRaModem_h
#define LoRaModem_h

#include <LoRaPacket.h>

//this class represents the ATT cloud platform.
class LoRaModem
{
	public:
		// Returns the required baudrate for the device
		virtual unsigned int getDefaultBaudRate() = 0;
		//stop the modem.
		virtual void Stop() = 0;
		//set the modem in LoRaWan mode (vs private networks)
		virtual void SetLoRaWan(bool adr = true) = 0;
		//assign a device address to the modem
		//devAddress must be 4 bytes long
		virtual void SetDevAddress(unsigned char* devAddress) = 0;
		//set the app session key for the modem communication
		//app session key must be 16 bytes long
		virtual void SetAppKey(unsigned char* appKey) = 0;
		//set the network session key
		//network session key must be 16 bytes long
		virtual void SetNWKSKey(unsigned char*  nwksKey) = 0;
		//start the modem: returns true if successful
		virtual bool Start() = 0;
		//send a data packet to the server
		//ack = true -> request ack
		virtual bool Send(LoRaPacket* packet, bool ack = true) = 0;
		//process any incoming packets from the modem
		virtual void ProcessIncoming() = 0;
};

#endif