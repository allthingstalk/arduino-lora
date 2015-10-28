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

#define DEFAULT_PAYLOAD_SIZE 52
#define PORT 1
#define DEFAULT_INPUT_BUFFER_SIZE 96
#define DEFAULT_RECEIVED_PAYLOAD_BUFFER_SIZE 64
#define DEFAULT_TIMEOUT 120
#define RECEIVE_TIMEOUT 60000
#define MAX_SEND_RETRIES 10


enum MacTransmitErrorCodes
{
	NoError = 0,
	NoResponse = 1,
	Timeout = 2,
	TransmissionFailure = 3
};

//this class represents the ATT cloud platform.
class MicrochipLoRaModem: public LoRaModem
{
	public:
		//create the object
		MicrochipLoRaModem(Stream* stream);
		// Returns the required baudrate for the device
		unsigned int getDefaultBaudRate();
		//stop the modem.
		void Stop();
		//set the modem in LoRaWan mode (vs private networks)
		//adr = adaptive data rate. true= use, false = none adaptive data rate
		void SetLoRaWan(bool adr = true);
		//assign a device address to the modem
		//devAddress must be 4 bytes long
		void SetDevAddress(unsigned char* devAddress);
		//set the app session key for the modem communication
		//app session key must be 16 bytes long
		void SetAppKey(unsigned char* appKey);
		//set the network session key
		//network session key must be 16 bytes long
		void SetNWKSKey(unsigned char*  nwksKey);
		//start the modem , returns true if successful
		bool Start();
		//send a data packet to the server
		bool Send(LoraPacket* packet, bool ack = true);
		//process any incoming packets from the modem
		 void ProcessIncoming();
	private:
		Stream* _stream;					//the stream to communicate with the lora modem.
		char inputBuffer[DEFAULT_INPUT_BUFFER_SIZE + 1];
	    char receivedPayloadBuffer[DEFAULT_RECEIVED_PAYLOAD_BUFFER_SIZE + 1];
		unsigned char lookupMacTransmitError(const char* error);
		unsigned char onMacRX();
		void printHex(unsigned char hex);
		unsigned short readLn(char* buf, unsigned short bufferSize, unsigned short start = 0);
		unsigned short readLn() { return readLn(this->inputBuffer, DEFAULT_INPUT_BUFFER_SIZE); };
		bool expectOK();
		bool expectString(const char* str, unsigned short timeout = DEFAULT_TIMEOUT);
		
		bool setMacParam(const char* paramName, const unsigned char* paramValue, unsigned short size);
		bool setMacParam(const char* paramName, unsigned char paramValue);
		bool setMacParam(const char* paramName, const char* paramValue);
		unsigned char macTransmit(const char* type, const unsigned char* payload, unsigned char size);
};

#endif