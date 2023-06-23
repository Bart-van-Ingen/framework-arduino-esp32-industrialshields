/*
   Copyright (c) 2023 Boot&Work Corp., S.L.

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __INDUSTRIALSHIELDS_RS232_H__
#define __INDUSTRIALSHIELDS_RS232_H__

#include <Arduino.h>

#if defined(HAVE_RS232)

#if defined(HAVE_SERIAL_SC_0)

#include <HardwareSerialSC.h>

class RS232Class : public SC16IS752HardwareSerial {
	public:
		RS232Class(uint32_t spi_cs, uint8_t channel);
};

#endif // HAVE_RS232_HARD

extern RS232Class RS232;

#endif // HAVE_RS232

#endif // __INDUSTRIALSHIELDS_RS232_H__
