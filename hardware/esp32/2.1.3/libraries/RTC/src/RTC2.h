#ifndef __RTC_H__
#define __RTC_H__

#include <Arduino.h>

#if defined HAVE_RTC

class RTCClass {
	public:
		explicit RTCClass();

	public:
		bool read();
		bool write();

		inline bool isActive() const { return _active; }

		// UNIX timestamp (spent seconds since Jan 1 1970 00:00:00)
		uint32_t getTime() const;
		void setTime(uint32_t time);

		// Calendar
		inline uint8_t getSecond() const { return _second; }
		inline uint8_t getMinute() const { return _minute; }
		inline uint8_t getHour() const { return _hour; }
		inline uint8_t getWeekDay() const { return _weekDay; }
		inline uint8_t getMonthDay() const { return _monthDay; }
		inline uint8_t getMonth() const { return _month; }
		inline uint16_t getYear() const { return _year; }

		inline void setSecond(uint8_t second) { _second = second; }
		inline void setMinute(uint8_t minute) { _minute = minute; }
		inline void setHour(uint8_t hour) { _hour = hour; }
		inline void setWeekDay(uint8_t weekDay) { _weekDay = weekDay; }
		inline void setMonthDay(uint8_t monthDay) { _monthDay = monthDay; }
		inline void setMonth(uint8_t month) { _month = month; }
		inline void setYear(uint16_t year) { _year = year; }

	private:
		bool _active;
		uint8_t _second;
		uint8_t _minute;
		uint8_t _hour;
		uint8_t _weekDay;
		uint8_t _monthDay;
		uint8_t _month;
		uint16_t _year;
};

extern RTCClass RTC;

#endif // HAVE_RTC

#endif // __RTC_H__
