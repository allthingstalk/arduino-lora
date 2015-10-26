/*********************************************************************
  This is a library for the Grove Air Quality Sensor
  based on ideas by the AirQuality library from Seeedstudio

  Written by Michael Haugk
  Version 1.0
  Date Nov. 29th, 2014  
  BSD license, all text above must be included in any redistribution
 *********************************************************************/

#ifndef AIRQUALITY2_H
#define AIRQUALITY2_H

#define BAD_AIR_THRESHOLD 700
#define HIGH_POLLUTION_INDICATOR 150
#define LOW_POLLUTION_INDICATOR 50

class AirQuality2 {
 public:
  AirQuality2();
  int init(int Pin);
  int evaluate();
  int getRawData();
  int getReferenceValue();
  
 private:
  bool isInitialized;
  int iArduinoPin;
  int iReferenceValue;
  void calcReference(int sValue);
};


#endif //  AIRQUALITY2_H
