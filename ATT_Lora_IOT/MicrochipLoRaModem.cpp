/*
	MicrochipLoRaModem.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "MicrochipLoRaModem.h"
#include "StringLiterals.h"
#include "Utils.h"
#include <Arduino.h>

#define PORT 0x01
//#define FULLDEBUG

#define PACKET_TIME_OUT 45000


typedef struct StringEnumPair
{
	const char* stringValue;
	uint8_t enumValue;
} StringEnumPair;

unsigned char microchipSendBuffer[DEFAULT_PAYLOAD_SIZE];

MicrochipLoRaModem::MicrochipLoRaModem(Stream* stream, Stream *monitor)
{
	_stream = stream;
	_monitor = monitor;
}

bool MicrochipLoRaModem::Stop()
{
#ifdef FULLDEBUG
	PRINTLN("[resetDevice]");
#endif

	_stream->print(STR_CMD_RESET);
	_stream->print(CRLF);

	return expectString(STR_DEVICE_TYPE);
}

bool MicrochipLoRaModem::SetLoRaWan(bool adr)
{
	//lorawan should be on by default (no private supported)
	return setMacParam(STR_ADR, BOOL_TO_ONOFF(adr));			//set to adaptive variable rate transmission
}

unsigned int MicrochipLoRaModem::getDefaultBaudRate() 
{ 
	return 57600; 
};

bool MicrochipLoRaModem::SetDevAddress(unsigned char* devAddress)
{
#ifdef FULLDEBUG
	PRINTLN("Setting the DevAddr");
#endif	
	return setMacParam(STR_DEV_ADDR, devAddress, 4); 
}

bool MicrochipLoRaModem::SetAppKey(unsigned char* appKey)
{
	#ifdef FULLDEBUG
	PRINTLN("Setting the AppSKey"); 
	#endif	
	return setMacParam(STR_APP_SESSION_KEY, appKey, 16);
}

bool MicrochipLoRaModem::SetNWKSKey(unsigned char*  nwksKey)
{
	#ifdef FULLDEBUG
	PRINTLN("Setting the NwkSKey"); 
	#endif	
	return setMacParam(STR_NETWORK_SESSION_KEY, nwksKey, 16);
}

bool MicrochipLoRaModem::Start()
{
	#ifdef FULLDEBUG
	PRINTLN("Sending the network start commands");
	#endif		
	_stream->print(STR_CMD_JOIN);
	_stream->print(STR_ABP);
	_stream->print(CRLF);

	#ifdef FULLDEBUG	
	PRINT(STR_CMD_JOIN);
	PRINT(STR_ABP);
	PRINT(CRLF);
	#endif

	return expectOK() && expectString(STR_ACCEPTED);
}

bool MicrochipLoRaModem::Send(LoRaPacket* packet, bool ack)
{
	unsigned char length = packet->Write(microchipSendBuffer);
	PRINTLN("Sending payload: ");
	for (unsigned char i = 0; i < length; i++) {
		printHex(microchipSendBuffer[i]);
	}
	PRINTLN();
		
	unsigned char result;
	if(ack == true){
		if (!setMacParam(STR_RETRIES, MAX_SEND_RETRIES))		// not a fatal error -just show a debug message
			PRINTLN("[send] Non-fatal error: setting number of retries failed.");
		result = macTransmit(STR_CONFIRMED, microchipSendBuffer, length) == NoError;
	}
	else{
		result = macTransmit(STR_UNCONFIRMED, microchipSendBuffer, length) == NoError;
	}
	if(result){
		PRINTLN("Successfully sent packet");
	}
	else{
		PRINTLN("Failed to send packet");
	}
	return result;
}


unsigned char MicrochipLoRaModem::macTransmit(const char* type, const unsigned char* payload, unsigned char size)
{
	_stream->print(STR_CMD_MAC_TX);
	_stream->print(type);
	_stream->print(PORT);
	_stream->print(" ");
	
	#ifdef FULLDEBUG	
	PRINT(STR_CMD_MAC_TX);
	PRINT(type);
	PRINT(PORT);
	PRINT(" ");
	#endif

	for (int i = 0; i < size; ++i)
	{
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
		#ifdef FULLDEBUG		
		PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
		PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
		#endif
	}
	_stream->print(CRLF);

	#ifdef FULLDEBUG	
	PRINT(CRLF);
	#endif

	// TODO lookup error
	if (!expectOK())
		return TransmissionFailure;

	#ifdef FULLDEBUG
	PRINTLN("Waiting for server response");
	#endif
	unsigned long timeout = millis() + RECEIVE_TIMEOUT;
	while (millis() < timeout)
	{
		#ifdef FULLDEBUG	
		PRINT(".");
		#endif
		if (readLn() > 0)
		{
			#ifdef FULLDEBUG		
			PRINT(".(");
			PRINT(this->inputBuffer);
			PRINT(")");
			#endif

			if (strstr(this->inputBuffer, " ") != NULL) // to avoid double delimiter search 
			{
				// there is a splittable line -only case known is mac_rx
				#ifdef FULLDEBUG				
				PRINTLN("Splittable response found");
				#endif
				onMacRX();
				return NoError; // TODO remove
			}
			else if (strstr(this->inputBuffer, STR_RESULT_MAC_TX_OK))
			{
				// done
				#ifdef FULLDEBUG
				PRINTLN("Received mac_tx_ok");
				#endif
				return NoError;
			}
			else
			{
				#ifdef FULLDEBUG
				PRINTLN("Some other string received (error)");
				#endif
				return lookupMacTransmitError(this->inputBuffer);
			}
		}
	}
	PRINTLN("Timed-out waiting for a response!");
	return Timeout;
}

uint8_t MicrochipLoRaModem::lookupMacTransmitError(const char* error)
{
	PRINT("[lookupMacTransmitError]: ");
	PRINTLN(error);

	if (error[0] == 0)
	{
		PRINTLN("[lookupMacTransmitError]: The string is empty!");
		return NoResponse;
	}

	StringEnumPair errorTable[] =
	{
		{ STR_RESULT_OK, NoError },
		{ STR_RESULT_INVALID_PARAM, TransmissionFailure },
		{ STR_RESULT_NOT_JOINED, TransmissionFailure },
		{ STR_RESULT_NO_FREE_CHANNEL, TransmissionFailure },
		{ STR_RESULT_SILENT, TransmissionFailure },
		{ STR_RESULT_FRAME_COUNTER_ERROR, TransmissionFailure },
		{ STR_RESULT_BUSY, TransmissionFailure },
		{ STR_RESULT_MAC_PAUSED, TransmissionFailure },
		{ STR_RESULT_INVALID_DATA_LEN, TransmissionFailure },
		{ STR_RESULT_MAC_ERROR, TransmissionFailure },
		{ STR_RESULT_MAC_TX_OK, NoError }
	};

	for (StringEnumPair* p = errorTable; p->stringValue != NULL; ++p) 
	{
		if (strcmp(p->stringValue, error) == 0)
		{
			PRINT("[lookupMacTransmitError]: found ");
			PRINTLN(p->enumValue);

			return p->enumValue;
		}
	}

	PRINTLN("[lookupMacTransmitError]: not found!");
	return NoResponse;
}


// waits for string, if str is found returns ok, if other string is found returns false, if timeout returns false
bool MicrochipLoRaModem::expectString(const char* str, unsigned short timeout)
{
	#ifdef FULLDEBUG
	PRINT("[expectString] expecting ");
	PRINTLN(str);
	#endif

	unsigned long start = millis();
	while (millis() < start + timeout)
	{
		#ifdef FULLDEBUG	
		PRINT(".");
		#endif

		if (readLn() > 0)
		{
			#ifdef FULLDEBUG		
			PRINT("(");
			PRINT(this->inputBuffer);
			PRINT(")");
			#endif

			// TODO make more strict?
			if (strstr(this->inputBuffer, str) != NULL)
			{
				#ifdef FULLDEBUG
				PRINTLN(" found a match!");
				#endif
				return true;
			}
			return false;
		}
	}

	return false;
}

unsigned short MicrochipLoRaModem::readLn(char* buffer, unsigned short size, unsigned short start)
{
	int len = _stream->readBytesUntil('\n', buffer + start, size);
	if(len > 0)
		this->inputBuffer[start + len - 1] = 0; // bytes until \n always end with \r, so get rid of it (-1)
	else
		this->inputBuffer[start] = 0;

	return len;
}

bool MicrochipLoRaModem::expectOK()
{
	return expectString(STR_RESULT_OK);
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, const unsigned char* paramValue, unsigned short size)
{
	#ifdef FULLDEBUG
	PRINT("[setMacParam] ");
	PRINT(paramName);
	PRINT("= [array]");
	#endif

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	#ifdef FULLDEBUG
	PRINT(STR_CMD_SET);
	PRINT(paramName);
	#endif

	for (unsigned short i = 0; i < size; ++i)
	{
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))));
		#ifdef FULLDEBUG
		PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
		PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i])))); 
		#endif
	}
	
	_stream->print(CRLF);
	#ifdef FULLDEBUG
	PRINT(CRLF);
	#endif

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, uint8_t paramValue)
{
	#ifdef FULLDEBUG
	PRINT("[setMacParam] ");
	PRINT(paramName);
	PRINT("= ");
	PRINTLN(paramValue);
	#endif

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);
	
	#ifdef FULLDEBUG
	PRINT(STR_CMD_SET);
	PRINT(paramName);
	PRINT(paramValue);
	PRINT(CRLF);
	#endif

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, const char* paramValue)
{
	//PRINT("[setMacParam] ");
	//PRINT(paramName);
	//PRINT("= ");
	//PRINTLN(paramValue);

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);
	
	#ifdef FULLDEBUG
	PRINT(STR_CMD_SET);
	PRINT(paramName);
	PRINT(paramValue);
	PRINT(CRLF);
	#endif
	
	return expectOK();
}


//process any incoming packets from the modem
 void MicrochipLoRaModem::ProcessIncoming()
 {
	readLn();
 }

void MicrochipLoRaModem::printHex(unsigned char hex)
{
  char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  PRINT(hexTable[hex /16]);
  PRINT(hexTable[hex % 16]);
  PRINT(' ');
}

unsigned char MicrochipLoRaModem::onMacRX()
{

	// parse inputbuffer, put payload into packet buffer
	char* token = strtok(this->inputBuffer, " ");

	// sanity check
	if (strcmp(token, STR_RESULT_MAC_RX) != 0){
		PRINTLN("no mac_rx found in result");
		return NoResponse; // TODO create more appropriate error codes
	}

	// port
	token = strtok(NULL, " ");

	// payload
	token = strtok(NULL, " "); // until end of string
	memcpy(this->receivedPayloadBuffer, token, strlen(token) + 1); // TODO check for buffer limit

	return NoError;
}

//extract the specified instrumentation parameter from the modem and return the value
int MicrochipLoRaModem::GetParam(instrumentationParam param)
{
	switch(param){
		case MODEM: return 0;
		case FREQUENCYBAND:{
			if (strstr(getMacParam("band"), "868")) return 1;
			return 0;}
		case SP_FACTOR:{
			char* val = getRadioParam("sf");
			return sfToIndex(val);}
		case ADR:{
			if (strstr(getMacParam("adr"), "on")) return 1;
			return 0;}
		case POWER_INDEX:{
			char* val = getMacParam("pwridx");
			return atoi(val);}
		case BANDWIDTH:{
			char* val = getRadioParam("bw");
			if (strstr(val, "500")) return 3;
			if (strstr(val, "250")) return 2;
			if (strstr(val, "125")) return 1;
			return 0;}
		case CODING_RATE:{
			char* val = getRadioParam("cr");
			if (strstr(val, "4/8")) return 3;
			if (strstr(val, "4/7")) return 2;
			if (strstr(val, "4/6")) return 1;
			return 0;}
		case DUTY_CYCLE:{
			char* val = getMacParam("dcycleps");
			return atoi(val);}
		case SNR:{
			char* val = getRadioParam("snr");
			return atoi(val);}
		case GATEWAY_COUNT:{
			char* val = getMacParam("gwnb");
			return atoi(val);}
		case RETRANSMISSION_COUNT:{
			char* val = getMacParam("retx");
			return atoi(val);}
		case DATA_RATE:{
			char* val = getMacParam("dr");
			return atoi(val);}
		default: return false;
	}
}

char* MicrochipLoRaModem::getMacParam(const char* paramName, unsigned short timeout)
{
	#ifdef FULLDEBUG	
	PRINT("[getMacParam] ");
	PRINT(paramName);
	#endif

	_stream->print(STR_CMD_GET_MAC);
	_stream->print(paramName);	
	_stream->print(CRLF);
	
	unsigned long start = millis();
	while (millis() < start + timeout)
	{
		if (readLn() > 0)
		{
			#ifdef FULLDEBUG
			PRINT(" = ");
			PRINTLN(this->inputBuffer);
			#endif
			return this->inputBuffer;
		}
	}
	return NULL;						//no repsonse, so return empty string
}

char* MicrochipLoRaModem::getRadioParam(const char* paramName, unsigned short timeout)
{
	#ifdef FULLDEBUG	
	PRINT("[getRadioParam] ");
	PRINT(paramName);
	#endif

	_stream->print(STR_CMD_GET_RADIO);
	_stream->print(paramName);	
	_stream->print(CRLF);
	
	unsigned long start = millis();
	while (millis() < start + timeout)
	{
		if (readLn() > 0)
		{
			#ifdef FULLDEBUG
			PRINT(" = ");
			PRINTLN(this->inputBuffer);
			#endif
			return this->inputBuffer;
		}
	}
	return NULL;						//no repsonse, so return empty string
}

//convert the text value for spreading factor into a number between 0 and 6
int MicrochipLoRaModem::sfToIndex(char* value)
{
	int len = strlen(value);
	if(len == 3){
		int res = value[2] - 54;     			//48 = ascii 0,  ascii 55 = 7 -> transaltes to index 0x01
		if (res >= 1 && res < 3) return res;	//small sanity check, make certain that it is within the expected range
	}
	else if(len == 4){
		int res = value[3] - 47 + 3;
		if (res >= 4 && res < 7) return res;	//small sanity check, make certain that it is within the expected range
	}
	return 0;   //unknown spreading factor
}

//returns the id number of the modem type. See the container definition for the instrumentation container to see more details.
int MicrochipLoRaModem::GetModemId()
{
	return 3;
}
