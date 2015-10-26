/*
 * Sodaq_TPH.cpp
 *
 * This file is part of Sodaq_TPH.
 *
 * Sodaq_TPH is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or(at your option) any later version.
 *
 * Sodaq_TPH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sodaq_TPH.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <Wire.h>
#include <Sodaq_BMP085.h>
#include <Sodaq_SHT2x.h>
#include "Sodaq_TPH.h"

TPH tph;
Sodaq_BMP085 bmp;

void TPH::begin()
{
  Wire.begin();
  bmp.begin();
}

/*
 * Get the temperature in degrees Celsius (average of SHT2x and BMP085 sensors)
 */
float TPH::readTemperature()
{
  float result = 0;
  float temp;
  uint8_t count = 0;
  temp = readTemperatureBMP();
  if (temp > -200) {
    result += temp;
    ++count;
  }
  temp = SHT2x.GetTemperature();
  if (temp > -200) {
    result += temp;
    ++count;
  }
  if (count == 0) {
    return -273;
  }
  return result / count;
}

/*
 * Get the temperature in degrees Celsius from the BMP085 sensor
 */
float TPH::readTemperatureBMP()
{
  float temp = bmp.readTemperature();
  if (temp < -273) {
    temp = -273;
  }
  return temp;
}

/*
 * Get the air pressure in Pa
 */
int32_t TPH::readPressure()
{
  return bmp.readPressure();
}

/*
 * Get the temperature in degrees Celsius from the SHT2x sensor
 */
float TPH::readTemperatureSHT()
{
  return SHT2x.GetTemperature();
}

/*
 * Get the relative humidity in %RH
 */
float TPH::readHumidity()
{
  return SHT2x.GetHumidity();
}
