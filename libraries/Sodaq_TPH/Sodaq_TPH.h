/*
 * Sodaq_TPH.h
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

#ifndef SODAQ_TPH_H_
#define SODAQ_TPH_H_

class TPH
{
public:
  void begin();

  float readTemperature();
  int32_t readPressure();
  float readHumidity();

  float readTemperatureBMP();

  float readTemperatureSHT();
};

extern TPH tph;

#endif /* SODAQ_TPH_H_ */
