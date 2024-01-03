#pragma once

#include <string>

#define TIME_STEP_DEFINTION(NAME) \
	unsigned long NAME##s() const; \
	unsigned long NAME() const; \
	unsigned long progress_##NAME(unsigned long NAME##s);

namespace RYSIC::World
{

	struct Time
	{
		unsigned long tics = 0;

		unsigned long progress(unsigned long _tics, unsigned long seconds = 0, unsigned long minutes = 0,
			unsigned long hours = 0, unsigned long days = 0, unsigned long months = 0, unsigned long years = 0);

		TIME_STEP_DEFINTION(second)
		TIME_STEP_DEFINTION(minute)
		TIME_STEP_DEFINTION(hour)
		TIME_STEP_DEFINTION(day)
		TIME_STEP_DEFINTION(month)
		TIME_STEP_DEFINTION(year)

		const std::string string_short() const;
		const std::string string() const;
		const std::string string_long() const;
		const std::string string_date() const;
		const std::string string_date_long() const;

		operator unsigned long() const { return tics; }
	};

}