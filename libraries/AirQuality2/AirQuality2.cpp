/*********************************************************************
  This is a library for the Grove Air Quality Sensor
  based on ideas by the AirQuality library from Seeedstudio

  Written by Michael Haugk
  Version 1.0
  Date Nov. 29th, 2014  
  BSD license, all text above must be included in any redistribution
 *********************************************************************/
 
 #include "AirQuality2.h"
 #include "Arduino.h"
 
 //Constructor
AirQuality2:: AirQuality2()
{
  isInitialized = false;
}

/**
*
*  returns -1 if initialization failed
*  otherwise it returns the initialization value
*/
int AirQuality2::init(int Pin)
{
  int i, _tmpVal[3];
  
  isInitialized = false;
  
  iArduinoPin = Pin;
  pinMode(iArduinoPin,INPUT);
  
  iReferenceValue = 0;
  
  // read the first value after 5sec heat time
  // and wait 5sec to read more values
  for(int i=0;i<3;i++)
  {
    delay(5000);
    _tmpVal[i] = analogRead(iArduinoPin);
	iReferenceValue += _tmpVal[i];
  }
  
  iReferenceValue = iReferenceValue/3;
  
  // define a set of rules to exclude bad initialization
  // like too high or too low sensor values
  if(iReferenceValue > BAD_AIR_THRESHOLD || iReferenceValue < 10)
  {
	  return -1;
  }
  if( abs(_tmpVal[0] - iReferenceValue) > 15 ||
	  abs(_tmpVal[1] - iReferenceValue) > 15 ||
	  abs(_tmpVal[2] - iReferenceValue) > 15)
  {
    return -2;
  }
  
  isInitialized = true;
  return iReferenceValue;
}

/**
*
*  the following values will be returned
*  -1: failed to read sensor values
*   0: good air quality
*   1: low pollution
*   2: high pollution
*   3: very high pollution
*
*/
int AirQuality2::evaluate()
{
  int currVal, valDiff, quality;
  
  if(!isInitialized) return -1;
  
  quality = -1;
  currVal = analogRead(iArduinoPin);
  valDiff = currVal - iReferenceValue;
  
  if(currVal > BAD_AIR_THRESHOLD) 
  {
    quality = 3;
  }
  else if(valDiff > HIGH_POLLUTION_INDICATOR) 
  {
    quality = 2;
  }
  else if(valDiff > LOW_POLLUTION_INDICATOR)
  {
    quality = 1;
  }
  else
  {
    quality = 0;
  }
  
  // Update reference value and last value
  calcReference(currVal);
  
  return quality;
}

/**
*
*  Get the raw sensor reading data
*
*/
int AirQuality2::getRawData()
{
   int currVal;
   if(!isInitialized) return -1;
   
   currVal = analogRead(iArduinoPin);
   calcReference(currVal);
   
   return (currVal);
}

/**
*
*  Update the reference value in order to contribute to sensor "memory" effect
*
*/
void AirQuality2::calcReference(int sValue)
{
   // Don't take the value into account for the baseline if it is higher than
   // the low pollution indicator plus current reference
   if(sValue < (LOW_POLLUTION_INDICATOR + iReferenceValue))
   {
     iReferenceValue = (iReferenceValue + sValue)/2;
   }
}

/**
*
*  return reference value
*
*/
int AirQuality2::getReferenceValue()
{
  return iReferenceValue;
}