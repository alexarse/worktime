#pragma once

#include <string>

namespace tk {
class Time {
private:
	int hour, minute, second;

public:
	// Initialise with current time.
	Time();

	/// String format -> HH:MM:SS
	Time(const std::string& time_str);

	/// @param h Hours   (from 0 to 23).
	/// @param m Minutes (from 0 to 59).
	/// @param s Seconds (from 0 to 59).
	Time(int h, int m, int s);

	inline void SetHours(int h)
	{
		hour = h;
	}

	inline void SetMinutes(int m)
	{
		minute = m;
	}

	inline void SetSeconds(int s)
	{
		second = s;
	}

	inline int GetHours()
	{
		return hour;
	}

	inline int GetMinutes()
	{
		return minute;
	}

	inline int GetSeconds()
	{
		return second;
	}

	Time& operator+=(const Time& time);

	Time operator-(const Time& time) const;

	/// Convert to HH:MM::SS string.
	std::string ToString() const;

	/// Convert to HH:MM string.
	std::string ToStringHM() const;
};
}