Sodaq_TPH
=========

This is an Arduino library for the SODAQ TPH board. This is
a temperature, pressure, humidity board with a BMP085 and
a SHT21 sensor.

The Sodaq_TPH library is mainly a wrapper around the libraries
for the two sensors: Sodaq_BMP085 and Sodaq_SHT2x.

An example sketch
```
#include <Arduino.h>
#include <Sodaq_TPH.h>
#include <Wire.h>

void setup()
{
  tph.begin();
}

void loop()
{
  float temp = tph.readTemperature();

  float bmp_temp = tph.readTemperatureBMP();

  float sht_temp = tph.readTemperatureSHT();

  float hum = tph.readHumidity();

  int32_t pres = tph.readPressure();
}
```

Functions
---------
* begin - initialize the Wire, and the sensors
* readTemperature - read the temp from both sensors and returns the average
* readPressure - read the air pressure in Pascal (Pa)
* readHumidity - read the humidity in %RH

Extra Functions
---------------
* readTemperatureBMP - read the temp from the BMP085 sensor
* readTemperatureSHT - read the temp from the SHT21 sensor
