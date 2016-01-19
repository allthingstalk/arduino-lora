/*
	ATT_IOT.cpp - SmartLiving.io LoRa Arduino library 
*/

#include <ATT_LoRa_IOT.h>
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
	_modem->Stop();								//stop any previously running modems
	_modem->SetLoRaWan(adr);						//switch to LoRaWan mode instead of peer to peer
	_modem->SetDevAddress(devAddress);
	_modem->SetAppKey(appKey);
	_modem->SetNWKSKey(nwksKey);
	bool result = _modem->Start();								//start the modem up 
	if(result == true)
		Serial.println("modem initialized");
	else
		Serial.println("failed to initialize modem");
	return result;									//we have created a connection succesfully.
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
	Serial.println("instrumentation values:");
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
	Serial.print(name);
	Serial.print(": ");
	Serial.println(value);
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
		Serial.print("adhering to LoRa bandwith usage, delaying next message for ");
		Serial.print((_minTimeBetweenSend + _lastTimeSent - curTime)/1000); Serial.println(" seconds");
		//Serial.print("curTime = "); Serial.print(curTime); Serial.print(", prevTime = "); Serial.print(_lastTimeSent); Serial.print(", dif = ");
		//Serial.println(_minTimeBetweenSend + _lastTimeSent - curTime);
		delay(_minTimeBetweenSend + _lastTimeSent - curTime);
	}
	bool res = _modem->Send(data, ack);
	while(res == false && (nrRetries < _maxRetries || _maxRetries == -1)) 
	{
		nrRetries++;
		Serial.print("retry in "); Serial.print(_minTimeBetweenSend/1000); Serial.println(" seconds");
		delay(_minTimeBetweenSend);
		Serial.println("resending");
		res = _modem->Send(data, ack);
	}
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


