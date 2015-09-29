/*
	MicrochipLoRaModem.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "MicrochipLoRaModem.h"

#define PORT 0x01

#define PACKET_TIME_OUT 45000

#define CRLF "\r\n"

#define STR_RESULT_OK "ok"
#define STR_RESULT_INVALID_PARAM "invalid_param"
#define STR_RESULT_MAC_ERROR "mac_err"
#define STR_RESULT_MAC_RX "mac_rx"
#define STR_RESULT_MAC_TX_OK "mac_tx_ok"

#define STR_RESULT_NOT_JOINED "not_joined"
#define STR_RESULT_NO_FREE_CHANNEL "no_free_ch"
#define STR_RESULT_SILENT "silent"
#define STR_RESULT_FRAME_COUNTER_ERROR "frame_counter_err_rejoin_needed"
#define STR_RESULT_BUSY "busy"
#define STR_RESULT_MAC_PAUSED "mac_paused"
#define STR_RESULT_INVALID_DATA_LEN "invalid_data_len"

#define STR_CMD_RESET "sys reset"
#define STR_DEVICE_TYPE "RN2483"

#define STR_CMD_SET "mac set "
#define STR_RETRIES "retx "
#define STR_DEV_ADDR "devaddr "
#define STR_APP_SESSION_KEY "appskey "
#define STR_NETWORK_SESSION_KEY "nwkskey "
#define STR_DEV_EUI "deveui "
#define STR_APP_EUI "appeui "
#define STR_APP_KEY "appkey "
#define STR_ADR "adr "

#define STR_CMD_JOIN "mac join "
#define STR_OTAA "otaa"
#define STR_ABP "abp"
#define STR_ACCEPTED "accepted"

#define STR_CMD_MAC_TX "mac tx "
#define STR_CONFIRMED "cnf "
#define STR_UNCONFIRMED "uncnf "


#define BOOL_TO_ONOFF(b) (b ? "on" : "off")
#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)

#define HEX_CHAR_TO_NIBBLE(c) ((c >= 'A') ? (c - 'A' + 0x0A) : (c - '0'))
#define HEX_PAIR_TO_BYTE(h, l) ((HEX_CHAR_TO_NIBBLE(h) << 4) + HEX_CHAR_TO_NIBBLE(l))

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

unsigned char sendBuffer[52];

MicrochipLoRaModem::MicrochipLoRaModem(Stream* stream)
{
	_stream = stream;
}

void MicrochipLoRaModem::Stop()
{
	debugPrintLn("[resetDevice]");

	_stream->print(STR_CMD_RESET);
	_stream->print(CRLF);

	return expectString(STR_DEVICE_TYPE);
}

void MicrochipLoRaModem::SetLoRaWan()
{
	Serial.println("Setting the network preferences to LoRaWAN private network");
	setMacParam(STR_DEV_ADDR, sizeof(CMD_LORA_PRVNET));
}

void MicrochipLoRaModem::SetDevAddress(unsigned char* devAddress)
{
	Serial.println("Setting the DevAddr");
	setMacParam(STR_DEV_ADDR, devAddress, 4); 
}

void MicrochipLoRaModem::SetAppKey(unsigned char* appKey)
{
	Serial.println("Setting the AppSKey");   
	setMacParam(STR_APP_SESSION_KEY, appKey, 16);
}

void MicrochipLoRaModem::SetNWKSKey(unsigned char*  nwksKey)
{
	Serial.println("Setting the NwkSKey");
	setMacParam(CMD_NWKSKEY, nwksKey, 16);
}

void MicrochipLoRaModem::Start()
{
	Serial.println("Sending the network start commands");
	
	setMacParam(STR_ADR, appKey, 16);			//set to adaptive variable rate transmission
	
	_stream->print(STR_CMD_JOIN);
	_stream->print(type);
	_stream->print(CRLF);

	if(expectOK() && expectString(STR_ACCEPTED))
		Serial.println("success");
	else
		Serial.println("failure");
}

void MicrochipLoRaModem::Send(LoraPacket* packet)
{
	unsigned char length = packet->Write(sendBuffer);
	Serial.println("Sending payload: ");
	for (unsigned char i = 0; i < length; i++) {
		printHex(sendBuffer[i]);
	}
	Serial.println();
  
	SendPacket(CMD_SEND_PREFIX, sizeof(CMD_SEND_PREFIX), sendBuffer, length);
	unsigned char result = ReadPacket(3);
	if(result != 0)
		Serial.println("Failed to send packet");
}

void MicrochipLoRaModem::SendPacket(unsigned char* data, uint16_t length)
{
	Serial.print("Sending: ");
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
	Serial.println();
}

void MicrochipLoRaModem::SendPacket(unsigned char* data, uint16_t length, unsigned char* data2, uint16_t length2)
{
	Serial.print("Sending: ");
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
	Serial.println();
}

void MicrochipLoRaModem::sendByte(unsigned char data)
{
	_stream->write(data);
	printHex(data);
}

void MicrochipLoRaModem::ReadPacket()
{
	uint32_t maxTS = millis() + PACKET_TIME_OUT;
	uint16_t length = 4;
	unsigned char firstByte = 0;
  
	Serial.print("Receiving: "); 

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
		Serial.print("Timeout");
	}
	Serial.println();
}

// waits for string, if str is found returns ok, if other string is found returns false, if timeout returns false
bool MicrochipLoRaModem::expectString(const char* str, uint16_t timeout)
{
	debugPrint("[expectString] expecting ");
	debugPrint(str);

	unsigned long start = millis();
	while (millis() < start + timeout)
	{
		debugPrint(".");

		if (readLn() > 0)
		{
			debugPrint("(");
			debugPrint(this->inputBuffer);
			debugPrint(")");

			// TODO make more strict?
			if (strstr(this->inputBuffer, str) != NULL)
			{
				debugPrintLn(" found a match!");

				return true;
			}

			return false;
		}
	}

	return false;
}

bool MicrochipLoRaWAN::expectOK()
{
	return expectString(STR_RESULT_OK);
}

// paramName should include the trailing space
bool MicrochipLoRaWAN::setMacParam(const char* paramName, const uint8_t* paramValue, uint16_t size)
{
	//debugPrint("[setMacParam] ");
	//debugPrint(paramName);
	//debugPrint("= [array]");

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);

	for (uint16_t i = 0; i < size; ++i)
	{
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
		_stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))));
	}
	
	this->loraStream->print(CRLF);

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaWAN::setMacParam(const char* paramName, uint8_t paramValue)
{
	//debugPrint("[setMacParam] ");
	//debugPrint(paramName);
	//debugPrint("= ");
	//debugPrintLn(paramValue);

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);

	return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaWAN::setMacParam(const char* paramName, const char* paramValue)
{
	//debugPrint("[setMacParam] ");
	//debugPrint(paramName);
	//debugPrint("= ");
	//debugPrintLn(paramValue);

	_stream->print(STR_CMD_SET);
	_stream->print(paramName);
	_stream->print(paramValue);
	_stream->print(CRLF);
	
	return expectOK();
}


//process any incoming packets from the modem
 void MicrochipLoRaModem::ProcessIncoming()
 {
	ReadPacket();
 }

void MicrochipLoRaModem::printHex(unsigned char hex)
{
  char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  Serial.print(hexTable[hex /16]);
  Serial.print(hexTable[hex % 16]);
  Serial.print(' ');
}
