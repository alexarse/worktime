#pragma once

#include <string>

namespace tk {
class Date {
private:
	int _year;
	int _month;
	int _day;

public:
	Date();
	
	Date(int year, int month, int day);
	
	/// String format -> yyyy-mm-dd
	Date(const std::string& date);
	
	int GetYear() const
	{
		return _year;
	}
	
	int GetMonth() const
	{
		return _month;
	}
	
	int GetDay() const
	{
		return _day;
	}
	
	void SetYear(int y)
	{
		_year = y;
	}
	
	void SetMonth(int m)
	{
		_month = m;
	}
	
	void SetDay(int d)
	{
		_day = d;
	}
	
	std::string ToStringFull() const;
	
	std::string ToString() const;
};
}
