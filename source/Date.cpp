#include "Date.hpp"

#include <chrono>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace tk {
Date::Date()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	_year = 1900 + ltm->tm_year;
	_month = 1 + ltm->tm_mon;
	_day = ltm->tm_mday;
}

Date::Date(int year, int month, int day)
	: _year(year)
	, _month(month)
	, _day(day)
{
}

Date::Date(const std::string& date_str)
	: _year(0)
	, _month(0)
	, _day(0)
{
	_year = std::stoi(date_str.substr(0, 4));
	_month = std::stoi(date_str.substr(5, 7));
	_day = std::stoi(date_str.substr(8, std::string::npos));
}

std::string Date::ToStringFull() const
{
	//	boost::gregorian::date d(boost::gregorian::from_simple_string(date_in));
	boost::gregorian::date d(_year, _month, _day);
	return std::string(d.day_of_week().as_long_string()) + " " + std::string(d.month().as_long_string()) + " "
		+ std::to_string(d.day());
}

std::string Date::ToString() const
{
	//	boost::gregorian::date d(boost::gregorian::from_simple_string(date_in));
	boost::gregorian::date d(_year, _month, _day);
	return boost::gregorian::to_iso_extended_string(d);
}
}