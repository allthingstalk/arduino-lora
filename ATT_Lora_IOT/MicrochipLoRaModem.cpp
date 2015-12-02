/*
	MicrochipLoRaModem.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "MicrochipLoRaModem.h"
#include "StringLiterals.h"
#include "Utils.h"

#define PORT 0x01

#define PACKET_TIME_OUT 45000


typedef struct StringEnumPair
{
	const char* stringValue;
	uint8_t enumValue;
} StringEnumPair;

unsigned char microchipSendBuffer[DEFAULT_PAYLOAD_SIZE];

MicrochipLoRaModem::MicrochipLoRaModem(Stream* stream)
{
	_stream = stream;
}

void MicrochipLoRaModem::Stop()
{
#ifdef FULLDEBUG
	Serial.println("[resetDevice]");
#endif

	_stream->print(STR_CMD_RESET);
	_stream->print(CRLF);

	expectString(STR_DEVICE_TYPE);
}

void MicrochipLoRaModem::SetLoRaWan(bool adr)
{
	//lorawan should be on by default (no private supported)
	setMacParam(STR_ADR, BOOL_TO_ONOFF(adr));			//set to adaptive variable rate transmission
}

unsigned int MicrochipLoRaModem::getDefaultBaudRate() 
{ 
	return 57600; 
};

void MicrochipLoRaModem::SetDevAddress(unsigned char* devAddress)
{
#ifdef FULLDEBUG
	Serial.println("Setting the DevAddr");
#endif	
	setMacParam(STR_DEV_ADDR, devAddress, 4); 
}

void MicrochipLoRaModem::SetAppKey(unsigned char* appKey)
{
	#ifdef FULLDEBUG
	Serial.println("Setting the AppSKey"); 
	#endif	
	setMacParam(STR_APP_SESSION_KEY, appKey, 16);
}

void MicrochipLoRaModem::SetNWKSKey(unsigned char*  nwksKey)
{
	#ifdef FULLDEBUG
	Serial.println("Setting the NwkSKey"); 
	#endif	
	setMacParam(STR_NETWORK_SESSION_KEY, nwksKey, 16);
}

bool MicrochipLoRaModem::Start()
{
	#ifdef FULLDEBUG
	Serial.println("Sending the network start commands");
	#endif		
	_stream->print(STR_CMD_JOIN);
	_stream->print(STR_ABP);
	_stream->print(CRLF);

	#ifdef FULLDEBUG	
	Serial.print(STR_CMD_JOIN);
	Serial.print(STR_ABP);
	Serial.print(CRLF);
	#endif

	return expectOK() && expectString(STR_ACCEPTED);
}

bool MicrochipLoRaModem::Send(LoRaPacket* packet, bool ack)
{
	unsigned char length = packet->Write(microchipSendBuffer);
	Serial.println("Sending payload: ");
	for (unsigned char i = 0; i < length; i++) {
		printHex(microchipSendBuffer[i]);
	}
	Serial.println();
		
	unsigned char result;
	if(ack == true)
	{
		if (!setMacParam(STR_RETRIES, MAX_SEND_RETRIES))		// not a fatal error -just show a debug message
			Serial.println("[send] Non-fatal error: setting number of retries failed.");
		result = macTransmit(STR_CONFIRMED, microchipSendBuffer, length) == NoError;
	}
	else
		result = macTransmit(STR_UNCONFIRMED, microchipSendBuffer, length) == NoError;
	if(result)
		Serial.println("Successfully sent packet");
	else
		Serial.println("Failed to send packet");
	return result;
}


unsigned char MicrochipLoRaModem::macTransmit(const char* type, const unsigned char* payload, unsigned char size)
{
	_stream->print(STR_CMD_MAC_TX);
	_stream->print(type);
	_stream->print(PORT);
	_stream->print(" ");
	
	#ifdef FULLDEBUG	
	Serial.print(STR_CMD_MAC_TX);
	Serial.print(type);
	Serial.print(PORT);
	Serial.print(" ");
	#endif

	for (int i = 0; i < size; ++i)
	{
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
		#ifdef FULLDEBUG		
		Serial.print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
		Serial.print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
		#endif
	}
	_stream->print(CRLF);

	#ifdef FULLDEBUG	
	Serial.print(CRLF);
	#endif

	// TODO lookup error
	if (!expectOK())
		return TransmissionFailure;

	#ifdef FULLDEBUG
	Serial.println("Waiting for server response");
	#endif
	unsigned long timeout = millis() + RECEIVE_TIMEOUT;
	while (millis() < timeout)
	{
		#ifdef FULLDEBUG	
		Serial.print(".");
		#endif
		if (readLn() > 0)
		{
			#ifdef FULLDEBUG		
			Serial.print(".");
			Serial.print("(");
			Serial.print(this->inputBuffer);
			Serial.print(")");
			#endif

			if (strstr(this->inputBuffer, " ") != NULL) // to avoid double delimiter search 
			{
				// there is a splittable line -only case known is mac_rx
				#ifdef FULLDEBUG				
				Serial.println("Splittable response found");
				#endif
				onMacRX();
				return NoError; // TODO remove
			}
			else if (strstr(this->inputBuffer, STR_RESULT_MAC_TX_OK))
			{
				// done
				#ifdef FULLDEBUG
				Serial.println("Received mac_tx_ok");
				#endif
				return NoError;
			}
			else
			{
				#ifdef FULLDEBUG
				Serial.println("Some other string received (error)");
				#endif
				return lookupMacTransmitError(this->inputBuffer);
			}
		}
	}
	Serial.println("Timed-out waiting for a response!");
	return Timeout;
}

uint8_t MicrochipLoRaModem::lookupMacTransmitError(const char* error)
{
	Serial.print("[lookupMacTransmitError]: ");
	Serial.println(error);

	if (error[0] == 0)
	{
		Serial.println("[lookupMacTransmitError]: The string is empty!");
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
			Serial.print("[lookupMacTransmitError]: found ");
			Serial.println(p->enumValue);

			return p->enumValue;
		}
	}

	Serial.println("[lookupMacTransmitError]: not found!");
	return NoResponse;
}


// waits for string, if str is found returns ok, if other string is found returns false, if timeout returns false
bool MicrochipLoRaModem::expectString(const char* str, unsigned short timeout)
{
	#ifdef FULLDEBUG
	Serial.print("[expectString] expecting ");
	Serial.println(str);
	#endif

	unsigned long start = millis();
	while (millis() < start + timeout)
	{
	#ifdef FULLDEBUG	
		Serial.print(".");
		#endif

		if (readLn() > 0)
		{
			#ifdef FULLDEBUG		
			Serial.print("(");
			Serial.print(this->inputBuffer);
			Serial.print(")");
			#endif

			// TODO make more strict?
			if (strstr(this->inputBuffer, str) != NULL)
			{
				#ifdef FULLDEBUG
				Serial.println(" found a match!");
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
	Serial.print("[setMacParam] ");
	Serial.print(paramName);
	Serial.print("= [array]");
	#endif

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	#ifdef FULLDEBUG
	Serial.print(STR_CMD_SET);
	Serial.print(paramName);
	#endif

	for (unsigned short i = 0; i < size; ++i)
	{
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))));
		#ifdef FULLDEBUG
		Serial.print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
		Serial.print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i])))); 
		#endif
	}
	
	_stream->print(CRLF);
	#ifdef FULLDEBUG
	Serial.print(CRLF);
	#endif

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, uint8_t paramValue)
{
	#ifdef FULLDEBUG
	Serial.print("[setMacParam] ");
	Serial.print(paramName);
	Serial.print("= ");
	Serial.println(paramValue);
	#endif

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);
	
	#ifdef FULLDEBUG
	Serial.print(STR_CMD_SET);
	Serial.print(paramName);
	Serial.print(paramValue);
	Serial.print(CRLF);
	#endif

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, const char* paramValue)
{
	//Serial.print("[setMacParam] ");
	//Serial.print(paramName);
	//Serial.print("= ");
	//Serial.println(paramValue);

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);
	
	#ifdef FULLDEBUG
	Serial.print(STR_CMD_SET);
	Serial.print(paramName);
	Serial.print(paramValue);
	Serial.print(CRLF);
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
  Serial.print(hexTable[hex /16]);
  Serial.print(hexTable[hex % 16]);
  Serial.print(' ');
}

unsigned char MicrochipLoRaModem::onMacRX()
{

	// parse inputbuffer, put payload into packet buffer
	char* token = strtok(this->inputBuffer, " ");

	// sanity check
	if (strcmp(token, STR_RESULT_MAC_RX) != 0){
	Serial.println("no mac_rx found in result");
		return NoResponse; // TODO create more appropriate error codes
	}

	// port
	token = strtok(NULL, " ");

	// payload
	token = strtok(NULL, " "); // until end of string
	memcpy(this->receivedPayloadBuffer, token, strlen(token) + 1); // TODO check for buffer limit

	return NoError;
}
