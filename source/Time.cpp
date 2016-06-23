#include "Time.hpp"
#include <chrono>
#include <sstream>

namespace tk {
Time::Time()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);
	
	hour = ltm->tm_hour;
	minute = ltm->tm_min;
	second = ltm->tm_sec;
}

Time::Time(const std::string& time_str)
	: hour(0)
	, minute(0)
	, second(0)
{
	hour = std::stoi(time_str.substr(0, 2));
	minute = std::stoi(time_str.substr(3, 5));
	second = std::stoi(time_str.substr(6, std::string::npos));
}

Time::Time(int h, int m, int s)
	: hour(h)
	, minute(m)
	, second(s)
{
}

Time& Time::operator+=(const Time& time)
{
	// Begin.
	std::chrono::hours h(hour);
	std::chrono::minutes m(minute);
	std::chrono::seconds s(second);

	std::chrono::seconds sec_begin = h + m + s;

	// End.
	std::chrono::hours h2(time.hour);
	std::chrono::minutes m2(time.minute);
	std::chrono::seconds s2(time.second);

	std::chrono::seconds sec_end = h2 + m2 + s2;

	std::chrono::seconds total = sec_begin + sec_end;

	std::chrono::hours hh = std::chrono::duration_cast<std::chrono::hours>(total);
	std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(total - hh);
	std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(total - hh - mm);

	hour = (int)hh.count();
	minute = (int)mm.count();
	second = (int)ss.count();

	return *this;
}

Time Time::operator-(const Time& time) const
{
	// Left.
	const std::chrono::hours h(hour);
	const std::chrono::minutes m(minute);
	const std::chrono::seconds s(second);
	
	// Left time in seconds.
	const std::chrono::seconds sec_left = h + m + s;
	
	// Right.
	const std::chrono::hours h2(time.hour);
	const std::chrono::minutes m2(time.minute);
	const std::chrono::seconds s2(time.second);
	
	// Right time in seconds.
	const std::chrono::seconds sec_right = h2 + m2 + s2;
	
	// Difference between left and right time.
	const std::chrono::seconds total = sec_left - sec_right;
	
	const std::chrono::hours hh = std::chrono::duration_cast<std::chrono::hours>(total);
	const std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(total - hh);
	const std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(total - hh - mm);
	
	return Time((int)hh.count(), (int)mm.count(), (int)ss.count());
}

std::string Time::ToString() const
{
	std::stringstream ss;

	if (hour < 10) {
		ss << "0";
	}

	ss << hour << ":";

	if (minute < 10) {
		ss << "0";
	}

	ss << minute << ":";

	if (second < 10) {
		ss << "0";
	}

	ss << second;
	
	return ss.str();
}

std::string Time::ToStringHM() const
{
	std::stringstream ss;

	if (hour < 10) {
		ss << "0";
	}

	ss << hour << ":";

	if (minute < 10) {
		ss << "0";
	}

	ss << minute;

	return ss.str();
}
}