/*
	EmbitLoRaModem.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "EmbitLoRaModem.h"

#define PORT 0x01

#define PACKET_TIME_OUT 45000

unsigned char CMD_STOP[1] = { 0x30 };
unsigned char CMD_LORA_PRVNET[2] = { 0x25, 0xA0 };
unsigned char CMD_DEV_ADDR[1] = { 0x21 };
unsigned char CMD_APPSKEY[2] = { 0x26, 0x11};
unsigned char CMD_NWKSKEY[2] = { 0x26, 0x10};
unsigned char CMD_START[1] = { 0x31 };
unsigned char CMD_SEND_PREFIX[4] = { 0x50, 0x0C, 0x00, PORT }; 

unsigned char sendBuffer[52];

EmbitLoRaModem::EmbitLoRaModem(Stream* stream)
{
	_stream = stream;
}

void EmbitLoRaModem::Stop()
{
	Serial.println("Sending the network stop command");
	SendPacket(CMD_STOP, sizeof(CMD_STOP));
	ReadPacket();
}

void EmbitLoRaModem::SetLoRaWan()
{
	Serial.println("Setting the network preferences to LoRaWAN private network");
	SendPacket(CMD_LORA_PRVNET, sizeof(CMD_LORA_PRVNET));
	ReadPacket();
}

void EmbitLoRaModem::SetDevAddress(unsigned char* devAddress)
{
	Serial.println("Setting the DevAddr");
	SendPacket(CMD_DEV_ADDR, sizeof(CMD_DEV_ADDR), devAddress, 4); 
	ReadPacket();
}

void EmbitLoRaModem::SetAppKey(unsigned char* appKey)
{
	Serial.println("Setting the AppSKey");   
	SendPacket(CMD_APPSKEY, sizeof(CMD_APPSKEY), appKey, 16);
	ReadPacket();
}

void EmbitLoRaModem::SetNWKSKey(unsigned char*  nwksKey)
{
	Serial.println("Setting the NwkSKey");
	SendPacket(CMD_NWKSKEY, sizeof(CMD_NWKSKEY), nwksKey, 16);
	ReadPacket();
}

void EmbitLoRaModem::Start()
{
	Serial.println("Sending the netowrk start command");
	SendPacket(CMD_START, sizeof(CMD_START));
	ReadPacket();
}

bool EmbitLoRaModem::Send(LoraPacket* packet)
{
	unsigned char length = packet->Write(sendBuffer);
	Serial.println("Sending payload: ");
	for (unsigned char i = 0; i < length; i++) {
		printHex(sendBuffer[i]);
	}
	Serial.println();
  
	SendPacket(CMD_SEND_PREFIX, sizeof(CMD_SEND_PREFIX), sendBuffer, length);
	unsigned char result = ReadPacket(3);
	if(result != 0){
		Serial.println("Failed to send packet");
		return false;
	}
	else
		return true;
	
}

void EmbitLoRaModem::SendPacket(unsigned char* data, uint16_t length)
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

void EmbitLoRaModem::SendPacket(unsigned char* data, uint16_t length, unsigned char* data2, uint16_t length2)
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

void EmbitLoRaModem::sendByte(unsigned char data)
{
	_stream->write(data);
	printHex(data);
}

void EmbitLoRaModem::ReadPacket()
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

unsigned char EmbitLoRaModem::ReadPacket(unsigned char index)
{
	uint32_t maxTS = millis() + PACKET_TIME_OUT;
	uint16_t length = 4;
	unsigned char firstByte = 0;
	unsigned char result = 0;
  
	Serial.print("Receiving: "); 

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
		Serial.print("Timeout");
	}
	Serial.println();
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
  Serial.print(hexTable[hex /16]);
  Serial.print(hexTable[hex % 16]);
  Serial.print(' ');
}
