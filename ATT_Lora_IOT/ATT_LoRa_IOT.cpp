/*
	ATT_IOT.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "ATT_LoRa_IOT.h"
//#include <arduino.h>


//create the object
ATTDevice::ATTDevice(LoRaModem* modem): _maxRetries(SEND_MAX_RETRY),  _minTimeBetweenSend(MIN_TIME_BETWEEN_SEND)
{
	_modem = modem;
	_lastTimeSent = 0;
}

//connect with the to the lora gateway
bool ATTDevice::Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr)
{
	if(!_modem->Stop()){								//stop any previously running modems
		Serial.println("can't communicate with modem: possible hardware issues");
		return false;
	}
	
	if (!_modem->SetLoRaWan(adr)){						//switch to LoRaWan mode instead of peer to peer				
		Serial.println("can't switch modem to lorawan mode: possible hardware issues?");
		return false;
	}
	if(!_modem->SetDevAddress(devAddress)){
		Serial.println("can't assign device address to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetAppKey(appKey)){
		Serial.println("can't assign app session key to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetNWKSKey(nwksKey)){
		Serial.println("can't assign network session key to modem: possible hardware issues?");
		return false;
	}
	bool result = _modem->Start();								//start the modem up 
	if(result == true)
		Serial.println("modem initialized, communication with base station established");
	else
		Serial.println("Modem is responding, but failed to communicate with base station. Possibly out of reach or invalid credentials.");
	return result;									//we have created a connection successfully.
}

//check for any incoming data
void ATTDevice::Process()
{
}


//send a data value to the cloud server for the sensor with the specified id.
bool ATTDevice::Send(String value, short id, bool ack)
{
	_data.Add(value);
	return Send(id, ack);
}

bool ATTDevice::Send(bool value, short id, bool ack)
{
	_data.Add(value);
	return Send(id, ack);
}

bool ATTDevice::Send(short value, short id, bool ack)
{
	_data.Add(value);
	return Send(id, ack);
}

bool ATTDevice::Send(float value, short id, bool ack)
{
	_data.Add(value);
	return Send(id, ack);
}

//sends the previously built complex data packet to the cloud for the sensor with the specified
bool ATTDevice::Send(short id, bool ack)
{
	_data.SetId(id);
	short nrRetries = 0;
	unsigned long curTime = millis();
	if(_lastTimeSent != 0 && _lastTimeSent + _minTimeBetweenSend > curTime)
	{
		Serial.print("adhering to LoRa bandwith usage, delaying next message for ");
		Serial.print((_minTimeBetweenSend + _lastTimeSent - curTime)/1000); Serial.println(" seconds");
		//Serial.print("curTime = "); Serial.print(curTime); Serial.print(", prevTime = "); Serial.print(_lastTimeSent); Serial.print(", dif = ");
		//Serial.println(_minTimeBetweenSend + _lastTimeSent - curTime);
		delay(_minTimeBetweenSend + _lastTimeSent - curTime);
	}
	bool res = _modem->Send(&_data, ack);
	while(res == false && (nrRetries < _maxRetries || _maxRetries == -1)) 
	{
		nrRetries++;
		Serial.print("retry in "); Serial.print(_minTimeBetweenSend/1000); Serial.println(" seconds");
		delay(_minTimeBetweenSend);
		Serial.println("resending");
		res = _modem->Send(&_data, ack);
	}
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
	_lastTimeSent = millis();
	return res;
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


