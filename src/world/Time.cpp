#include "Time.hpp"

#include <fmt/core.h>

#include "Constants.hpp"


#define TIME_STEP_IMPL(NAME, CONVERSION, BOUND_OP) \
	unsigned long Time::NAME##s() const { return tics / CONVERSION; } \
	unsigned long Time::NAME() const { return NAME##s() BOUND_OP; } \
	unsigned long Time::progress_##NAME(unsigned long NAME##s) \
	{ \
		long NAME##_tics = (NAME##s * Constants::TIME_TICS_SECOND); \
		progress(NAME##_tics); \
		return NAME##_tics; \
	}

namespace RYSIC::World
{
	
	unsigned long Time::progress(unsigned long _tics,
		unsigned long seconds,
		unsigned long minutes,
		unsigned long hours,
		unsigned long days,
		unsigned long months,
		unsigned long years)
	{
		unsigned long total_tics = _tics;
		if(years) total_tics += progress_year(years);
		if(months) total_tics += progress_month(months);
		if(days) total_tics += progress_day(days);
		if(hours) total_tics += progress_hour(hours);
		if(minutes) total_tics += progress_minute(minutes);
		if(seconds) total_tics += progress_second(seconds);
		tics += _tics;
		return total_tics;

	}

	TIME_STEP_IMPL(
		second,
		Constants::TIME_TICS_SECOND,
		% Constants::TIME_SECOND_MINUTE)
	TIME_STEP_IMPL(
		minute,
		Constants::TIME_TICS_MINUTE,
		% Constants::TIME_MINUTE_HOUR)
	TIME_STEP_IMPL(
		hour,
		Constants::TIME_TICS_HOUR,
		% Constants::TIME_HOUR_DAY)
	TIME_STEP_IMPL(
		day,
		Constants::TIME_TICS_DAY,
		% Constants::TIME_DAY_MONTH + 1)
	TIME_STEP_IMPL(
		month,
		Constants::TIME_TICS_MONTH,
		% Constants::TIME_MONTH_YEAR)
	TIME_STEP_IMPL(
		year,
		Constants::TIME_TICS_YEAR,
		+ 1)

	const std::string Time::string_short() const
	{
		return fmt::format(
			Constants::TIME_FORMAT_SHORT,
			hour(), minute(), second());
	}
	const std::string Time::string() const
	{
		return fmt::format(
			Constants::TIME_FORMAT,
			Constants::TIME_MONTH_NAMES[month()], day(), hour(), minute(), second());
	}
	const std::string Time::string_long() const
	{
		return fmt::format(
			Constants::TIME_FORMAT_LONG,
			Constants::TIME_MONTH_NAMES[month()], day(), year(), hour(), minute(), second());
	}

	const std::string Time::string_date() const
	{
		return fmt::format(
			Constants::TIME_FORMAT_DATE,
			Constants::TIME_MONTH_NAMES[month()], day(), year());
	}

	const std::string Time::string_date_long() const
	{
		return fmt::format(
			Constants::TIME_FORMAT_DATE_LONG,
			Constants::TIME_MONTH_NAMES[month()], day(), year());
	}

}