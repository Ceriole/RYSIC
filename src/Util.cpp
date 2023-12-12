#include "Util.hpp"

#include <fmt/core.h>

#include "Constants.hpp"


namespace RYSIC::Util
{
	std::string short_time_to_string(unsigned long time)
	{
		unsigned int seconds = (unsigned int) (time / Constants::WORLD_TICS_SECOND);
		unsigned int minutes = seconds / 60;
		unsigned int hours = minutes / 60;
		return tcod::stringf(Constants::WORLD_SHORT_TIME_FORMAT, hours % 24, minutes % 60, seconds % 60);
	}

	std::string time_to_string(unsigned long time)
	{
		unsigned int seconds = (unsigned int) (time / Constants::WORLD_TICS_SECOND);
		unsigned int minutes = seconds / 60;
		unsigned int hours = minutes / 60;
		unsigned int days = hours / 24;
		unsigned int years = days / 365;
		return tcod::stringf(Constants::WORLD_TIME_FORMAT, years % 999, days % 365, hours % 24, minutes % 60, seconds % 60);
	}
}