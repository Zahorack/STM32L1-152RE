/*
 * Time.h
 *
 *  Created on: Jun 25, 2018
 *      Author: xgallom
 */

#ifndef UTIL_TIME_H_
#define UTIL_TIME_H_

#include "stm32l1xx.h"

typedef int64_t micros_t;
typedef micros_t millis_t;
typedef millis_t seconds_t;
typedef seconds_t minutes_t;

namespace Util {

class Time {
	millis_t m_millis;

	explicit Time(micros_t micros = 0) : m_millis(micros) {}

public:
	Time(const Time &) = default;
	Time(Time &&) = default;

	inline static Time ZeroTime() { return Time(); }
//	inline static Time FromMicroSeconds(micros_t micros) { return Time(micros); }
	inline static Time FromMilliSeconds(millis_t millis) { return Time(millis); }
	inline static Time FromSeconds(seconds_t seconds) { return FromMilliSeconds(static_cast<millis_t>(seconds * 1000)); }
	inline static Time FromMinutes(minutes_t minutes) { return FromSeconds(static_cast<seconds_t>(minutes * 60)); }

//	inline micros_t microSeconds() const { return m_millis; }
	inline millis_t milliSeconds() const { return m_millis; }
	inline seconds_t seconds() const { return milliSeconds() / 1000; }
	inline minutes_t minutes() const { return seconds() / 60; }

	Time &operator =(const Time &) = default;
	Time &operator =(Time &&) = default;

	inline Time &operator +=(const Time &rhs) { m_millis += rhs.m_millis; return *this; }
	inline Time &operator -=(const Time &rhs) { m_millis -= rhs.m_millis; return *this; }

	friend Time operator +(Time lhs, const Time &rhs) { lhs += rhs; return lhs; }
	friend Time operator -(Time lhs, const Time &rhs) { lhs -= rhs; return lhs; }

	inline bool operator ==(const Time &rhs) { return m_millis == rhs.m_millis; }
	inline bool operator !=(const Time &rhs) { return m_millis != rhs.m_millis; }
	inline bool operator < (const Time &rhs) { return m_millis <  rhs.m_millis; }
	inline bool operator <=(const Time &rhs) { return m_millis <= rhs.m_millis; }
	inline bool operator > (const Time &rhs) { return m_millis >  rhs.m_millis; }
	inline bool operator >=(const Time &rhs) { return m_millis >= rhs.m_millis; }
};

} /* namespace Util */

#endif /* UTIL_TIME_H_ */
