/*************************************************** 
  AirQuality2 Example Code
  
  Written by Michael Haugk
  Version 1.0
  Date Nov. 29th, 2014  
  BSD license, all text above must be included in any redistribution
*/
#include <AirQuality2.h>
#include"Arduino.h"

AirQuality2 aqs;

void setup()
{
    int refVal;
    
    Serial.begin(9600);
    Serial.println("Initializing...");
    refVal = aqs.init(A0);
    
    if(refVal > 0) 
    {
      Serial.print("Reference value: ");
      Serial.println(refVal);
    }
    else
    {
      Serial.println("Failed to initialize sensor.");
      while(1);
    }
}

void loop()
{
    int result;
    
    result = aqs.evaluate();
    switch(result) 
    {
      case 0:
         Serial.println("Clean air.");
        break;
      case 1:
         Serial.println("Low pollution.");
        break;
      case 2:
         Serial.println("High pollution.");
        break;
      case 3:
        Serial.println("Very high pollution.");
        break;
      default:
        Serial.println("Error in sensor reading.");
    }    
    
    Serial.print("Sensor value: ");
    Serial.println(aqs.getRawData());
    Serial.print("Reference value: ");
    Serial.println(aqs.getReferenceValue());
    
    Serial.println();
    delay(2000);
}

