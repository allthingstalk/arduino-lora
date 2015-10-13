/*
	ATT_IOT.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "ATT_Lora_IOT.h"


//create the object
ATTDevice::ATTDevice(LoRaModem* modem)
{
	_modem = modem;
}

//connect with the to the lora gateway
bool ATTDevice::Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr)
{
	_modem->Stop();								//stop any previously running modems
	_modem->SetLoRaWan(adr);						//switch to LoRaWan mode instead of peer to peer
	_modem->SetDevAddress(devAddress);
	_modem->SetAppKey(appKey);
	_modem->SetNWKSKey(nwksKey);
	_modem->Start();								//start the modem up 
	return true;									//we have created a connection succesfully.
}

//check for any new mqtt messages.
//deAddress = 4 byte array
//appKey = 16 byte array
//nwksKey = 16 byte array
void ATTDevice::Process()
{
}


//send a data value to the cloud server for the sensor with the specified id.
void ATTDevice::Send(String value, short id, bool ack)
{
	_data.SetId(id);
	_data.Add(value);
	while(_modem->Send(&_data, ack) == false);
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
}

void ATTDevice::Send(bool value, short id, bool ack)
{
	_data.SetId(id);
	_data.Add(value);
	while(_modem->Send(&_data, ack) == false);
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
}

void ATTDevice::Send(short value, short id, bool ack)
{
	_data.SetId(id);
	_data.Add(value);
	while(_modem->Send(&_data, ack) == false);
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
}

void ATTDevice::Send(float value, short id, bool ack)
{
	_data.SetId(id);
	_data.Add(value);
	while(_modem->Send(&_data, ack) == false);
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
}

//sends the previously built complex data packet to the cloud for the sensor with the specified
void ATTDevice::Send(short id, bool ack)
{
	_data.SetId(id);
	while(_modem->Send(&_data, ack) == false);
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
}

//loads a bool data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(bool value)
{
	_data.Add(value);
}

//loads a bool data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(short value)
{
	_data.Add(value);
}

//loads a string data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(String value)
{
	_data.Add(value);
}

//loads a float data value into the data packet tha is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(float value)
{
	_data.Add(value);
}


