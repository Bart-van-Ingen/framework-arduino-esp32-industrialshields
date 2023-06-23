/*
   Copyright (c) 2017 Boot&Work Corp., S.L. All rights reserved

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RTC2.h"

#if defined HAVE_RTC
#if defined(HAVE_RTC_DS1307) || defined(HAVE_RTC_DS3231)

#include <BCD.h>
#include <Wire.h>

#define I2C_ADDRESS 0x68
#define I2C_DATA_ADDRESS 0x00
#define I2C_DATA_LENGTH 7

#define LEAP_YEAR(y) (!(((y) % 4) && (((y) % 100) || (!((y) % 400)))))
#define SECONDS_PER_MINUTE 60L
#define MINUTES_PER_HOUR 60L
#define SECONDS_PER_HOUR (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define HOURS_PER_DAY 24L
#define MINUTES_PER_DAY (MINUTES_PER_HOUR * HOURS_PER_DAY)
#define SECONDS_PER_DAY (SECONDS_PER_HOUR * HOURS_PER_DAY)

const uint8_t _monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

////////////////////////////////////////////////////////////////////////////////////////////////////
RTCClass::RTCClass() {
	Wire.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RTCClass::getTime() const {
	// Spent years
	uint32_t seconds = (uint32_t(_year) - 1970L) * SECONDS_PER_DAY * 365L;
	for (uint16_t i = 1970L; i < _year; ++i) {
		if (LEAP_YEAR(i)) {
			seconds +=  SECONDS_PER_DAY;
		}
	}

	// Spent months
	for (int i = 1; i < _month; i++) {
		seconds += SECONDS_PER_DAY * uint32_t(_monthDays[i - 1]);
		if ((i == 2) && LEAP_YEAR(_year)) {
			seconds += SECONDS_PER_DAY;
		}
	}

	// Spent days
	seconds += (uint32_t(_monthDay) - 1) * SECONDS_PER_DAY;

	seconds += _hour * SECONDS_PER_HOUR;
	seconds += _minute * SECONDS_PER_MINUTE;
	seconds += _second;
	return seconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RTCClass::setTime(uint32_t time) {
	_second = time % SECONDS_PER_MINUTE;
	time /= SECONDS_PER_MINUTE;
	_minute = time % MINUTES_PER_HOUR;
	time /= MINUTES_PER_HOUR;
	_hour = time % HOURS_PER_DAY;
	time /= HOURS_PER_DAY;

	_weekDay = ((time + 4) % 7) + 1;

	_year = 1970;
	uint32_t spentDays = 0;
	while (spentDays <= time) {
		spentDays += 365;
		if (LEAP_YEAR(_year)) {
			++spentDays;
		}
		++_year;
	}
	--_year;

	spentDays -= 365;
	if (LEAP_YEAR(_year)) {
		--spentDays;
	}
	time -= spentDays;

	for (_month = 1; _month <= 12; ++_month) {
		uint8_t monthLen = _monthDays[_month - 1];
		if ((_month == 2) && LEAP_YEAR(_year)) {
			++monthLen;
		}

		if (time >= monthLen) {
			time -= monthLen;
		} else {
			break;
		}
	}
	_monthDay = time + 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool RTCClass::read() {
	Wire.beginTransmission(I2C_ADDRESS);
	Wire.write((uint8_t) I2C_DATA_ADDRESS);
	if (Wire.endTransmission() != 0) {
		_active = false;
		return false;
	}

	Wire.requestFrom(I2C_ADDRESS, I2C_DATA_LENGTH);
	if (Wire.available() < I2C_DATA_LENGTH) {
		return false;
	}

	uint8_t second = Wire.read();
	_active = !(second & 0x80);
	_second = BCD2DEC(second & 0x7f);
	_minute = BCD2DEC(Wire.read());
	_hour = BCD2DEC(Wire.read() & 0x3f);
	_weekDay = BCD2DEC(Wire.read());
	_monthDay = BCD2DEC(Wire.read());
	_month = BCD2DEC(Wire.read());
	_year = BCD2DEC(Wire.read()) + 2000;

	return _active;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool RTCClass::write() {
	Wire.beginTransmission(I2C_ADDRESS);
	Wire.write((uint8_t) I2C_DATA_ADDRESS);
	Wire.write((uint8_t)0x80);
	Wire.write(DEC2BCD(_minute));
	Wire.write(DEC2BCD(_hour));
	Wire.write(DEC2BCD(_weekDay));
	Wire.write(DEC2BCD(_monthDay));
	Wire.write(DEC2BCD(_month));
	Wire.write(DEC2BCD(_year - 2000));
	if (Wire.endTransmission() != 0) {
		_active = false;
		return false;
	}

	Wire.beginTransmission(I2C_ADDRESS);
	Wire.write((uint8_t) I2C_DATA_ADDRESS);
	Wire.write(DEC2BCD(_second));
	if (Wire.endTransmission() != 0) {
		_active = false;
		return false;
	}

	_active = true;
	return true;
}

#endif // HAVE_RTC_DS1307

// RTC instance
RTCClass RTC;

#endif // HAVE_RTC
