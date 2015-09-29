/*
AllThingsTalk - SmartLiving.io Communicate with Embit lora modems through binary AT commands
Released into the public domain.

Original author: Jan Bogaerts (2015)
*/

#ifndef EmbitLoRaModem_h
#define EmbitLoRaModem_h

#include "LoRaModem.h"
#include "LoraPacket.h"
//#include <stream>

//this class represents the ATT cloud platform.
class MicrochipLoRaModem: public LoRaModem
{
	public:
		//create the object
		EmbitLoRaModem(Stream* stream);
		//stop the modem.
		void Stop();
		//set the modem in LoRaWan mode (vs private networks)
		void SetLoRaWan();
		//assign a device address to the modem
		//devAddress must be 4 bytes long
		void SetDevAddress(unsigned char* devAddress);
		//set the app session key for the modem communication
		//app session key must be 16 bytes long
		void SetAppKey(unsigned char* appKey);
		//set the network session key
		//network session key must be 16 bytes long
		void SetNWKSKey(unsigned char*  nwksKey);
		//start the modem 
		void Start();
		//send a data packet to the server
		void Send(LoraPacket* packet);
		//process any incoming packets from the modem
		 void ProcessIncoming();
	private:
		void printHex(unsigned char hex);
		void sendByte(unsigned char data);
		void SendPacket(unsigned char* data, uint16_t length);
		void SendPacket(unsigned char* data, uint16_t length, unsigned char* data2, uint16_t length2);
		void ReadPacket();
		//reads a packet from the modem and returns the value of the byte at the specified index position
		unsigned char ReadPacket(unsigned char index);
		
		
		bool expectOK();
		Stream* _stream;					//the stream to communicate with the lora modem.
};

#endif