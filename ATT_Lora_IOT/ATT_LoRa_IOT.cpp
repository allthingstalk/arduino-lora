/*
   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/  
#include <ATT_LoRa_IOT.h>
#include "Utils.h"
#include <arduino.h>			//still required for the 'delay' function. use #ifdef for other platforms.


//create the object
ATTDevice::ATTDevice(LoRaModem* modem, Stream* monitor):  _minTimeBetweenSend(MIN_TIME_BETWEEN_SEND)
{
	_modem = modem;
	 _monitor = monitor;
	_lastTimeSent = 0;
}

//connect with the to the lora gateway
bool ATTDevice::Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr)
{
	PRINT("ATT lib version: "); PRINTLN(VERSION);
	if(!_modem->Stop()){								//stop any previously running modems
		PRINTLN("can't communicate with modem: possible hardware issues");
		return false;
	}
	
	if (!_modem->SetLoRaWan(adr)){						//switch to LoRaWan mode instead of peer to peer				
		PRINTLN("can't set adr: possible hardware issues?");
		return false;
	}
	if(!_modem->SetDevAddress(devAddress)){
		PRINTLN("can't assign device address to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetAppKey(appKey)){
		PRINTLN("can't assign app session key to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetNWKSKey(nwksKey)){
		PRINTLN("can't assign network session key to modem: possible hardware issues?");
		return false;
	}
	bool result = _modem->Start();								//start the modem up 
	if(result == true){
		PRINTLN("modem initialized");
	}
	else{
		PRINTLN("Parameters loaded, but modem won't start: initialization failed");
	}
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

//collects all the instrumentation data from the modem (RSSI, ADR, datarate,..) and sends it over
//if ack = true -> request acknowledge, otherwise no acknowledge is waited for.
bool ATTDevice::SendInstrumentation(bool ack)
{
	PRINTLN("instrumentation values:");
	InstrumentationPacket data;
	SetInstrumentationParam(&data, MODEM, "modem", _modem->GetModemId());
	SetInstrumentationParam(&data, DATA_RATE, "data rate", _modem->GetParam(DATA_RATE));
	SetInstrumentationParam(&data, FREQUENCYBAND, "frequency band", _modem->GetParam(FREQUENCYBAND));
	SetInstrumentationParam(&data, POWER_INDEX, "power index", _modem->GetParam(POWER_INDEX));
	SetInstrumentationParam(&data, ADR, "ADR", _modem->GetParam(ADR));
	SetInstrumentationParam(&data, DUTY_CYCLE, "duty cycle", _modem->GetParam(DUTY_CYCLE));
	SetInstrumentationParam(&data, GATEWAY_COUNT, "nr of gateways", _modem->GetParam(GATEWAY_COUNT));
	SetInstrumentationParam(&data, SNR, "SNR", _modem->GetParam(SNR));
	SetInstrumentationParam(&data, SP_FACTOR, "spreading factor", _modem->GetParam(SP_FACTOR));
	SetInstrumentationParam(&data, BANDWIDTH, "bandwidth", _modem->GetParam(BANDWIDTH));
	SetInstrumentationParam(&data, CODING_RATE, "coding rate", _modem->GetParam(CODING_RATE));
	SetInstrumentationParam(&data, RETRANSMISSION_COUNT, "retransmission count", _modem->GetParam(RETRANSMISSION_COUNT));
	return Send(&data, ack);
}

//store the param in the  data packet, and print to serial.
void ATTDevice::SetInstrumentationParam(InstrumentationPacket* data, instrumentationParam param, char* name, int value)
{
	data->SetParam(param, value);
	PRINT(name);
	PRINT(": ");
	switch(param){
		case MODEM:
			if(value == 0){ PRINTLN("unknown");}
			else if(value == 1){ PRINTLN("multitech mdot");}
			else if(value == 2){ PRINTLN("embit-EMB-LR1272(E)");}
			else if(value == 3){ PRINTLN("microchip RN2483");}
			else {PRINT("unknown value: "); PRINTLN(value);}
			break;
		case BANDWIDTH:
			if(value == 0){ PRINTLN("unknown");}
			else if(value == 1){ PRINTLN("125");}
			else if(value == 2){ PRINTLN("250");}
			else if(value == 3){ PRINTLN("500");}
			else {PRINT("unknown value: "); PRINTLN(value);}
			break;
		case CODING_RATE:
			if(value == 0){ PRINTLN("4/5");}
			else if(value == 1){ PRINTLN("4/6");}
			else if(value == 2){ PRINTLN("4/7");}
			else if(value == 3){ PRINTLN("4/8");}
			else {PRINT("unknown value: "); PRINTLN(value);}
			break;
		case FREQUENCYBAND:
			if(value == 0){ PRINTLN("433");}
			else if(value == 1){ PRINTLN("868");}
			else {PRINT("unknown value: "); PRINTLN(value);}
			break;
		case SP_FACTOR:
			if(value == 0){ PRINTLN("unknown");}
			else if(value == 1){ PRINTLN("sf7");}
			else if(value == 2){ PRINTLN("sf8");}
			else if(value == 3){ PRINTLN("sf9");}
			else if(value == 4){ PRINTLN("sf10");}
			else if(value == 5){ PRINTLN("sf11");}
			else if(value == 6){ PRINTLN("sf12");}
			else {PRINT("unknown value: "); PRINTLN(value);}
			break;
		default: 
			PRINTLN(value);
			break;
	}
}

//sends the previously built complex data packet to the cloud for the sensor with the specified
bool ATTDevice::Send(short id, bool ack)
{
	_data.SetId(id);
	return Send(&_data, ack);
}

bool ATTDevice::Send(LoRaPacket* data, bool ack)
{
	short nrRetries = 0;
	unsigned long curTime = millis();
	if(_lastTimeSent != 0 && _lastTimeSent + _minTimeBetweenSend > curTime)
	{
		PRINT("adhering to LoRa bandwith usage, delaying next message for ");
		PRINT((_minTimeBetweenSend + _lastTimeSent - curTime)/1000); PRINTLN(" seconds");
		delay(_minTimeBetweenSend + _lastTimeSent - curTime);
	}
	bool res = _modem->Send(data, ack);
	data->Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
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


