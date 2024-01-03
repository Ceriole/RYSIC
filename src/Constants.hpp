#pragma once

#include <libtcod.hpp>

#include "Util.hpp"

namespace RYSIC::Constants
{

	// TILESET
	constexpr std::string_view
		DEFAULT_TILESET_FILE = "res/sprites.png";
	constexpr unsigned int
		TILESET_COLUMNS = 16,
		TILESET_ROWS = 32;
	constexpr std::array<int, 2>
		TILESET_COLUMNS_ROWS = {
				TILESET_COLUMNS,
				TILESET_ROWS
			};
	constexpr std::array<int, 16*4>
		CHARMAP_SPRITES = {
			0x3000, 0x3001, 0x3002, 0x3003, 0x3004, 0x3005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x3100, 0x3101, 0x3102, 0x3103, 0x3104, 0x3105, 0x3106, 0x3107, 0x3108, 0x3109, 0x310A, 0x310B, 0x310C, 0x0000, 0x0000, 0x0000,
			0x3200, 0x3201, 0x3202, 0x3203, 0x3204, 0x3205, 0x3206, 0x3207, 0x3208, 0x3209, 0x320A, 0x320B, 0x320C, 0x320D, 0x0000, 0x0000,
			0x4000, 0x4001, 0x4002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		};
	static const auto
		CHARMAP = RYSIC::Util::join(tcod::CHARMAP_CP437, CHARMAP_SPRITES);

	// SCREEN
	constexpr float
		DEFAULT_RATIO = 16.0f / 9.0f;
	constexpr unsigned int
		DEFAULT_WIDTH = 80,
		DEFAULT_HEIGHT = (unsigned int) (DEFAULT_WIDTH / DEFAULT_RATIO),
		DEFAULT_PIXEL_SIZE = 1;
	
	// MAP GEN
	constexpr int ROOM_GEN_MAX_ATTEMPTS = 10;

	// GRAPHICS
	constexpr float UNEXPLORED_DARK_FACTOR = 1.0f / 4.0f;

	// WORLD
	constexpr unsigned long
		TIME_TICS_SECOND = 100,										// tics/second
		TIME_SECOND_MINUTE = 60,									// second/minute
		TIME_TICS_MINUTE = TIME_TICS_SECOND * TIME_SECOND_MINUTE,	// tics/minute
		TIME_MINUTE_HOUR = 60,										// minutes/hour
		TIME_TICS_HOUR = TIME_TICS_MINUTE * TIME_MINUTE_HOUR,		// tics/hour
		TIME_HOUR_DAY = 24,											// hours/day
		TIME_TICS_DAY = TIME_TICS_HOUR * TIME_HOUR_DAY,				// tics/day
		TIME_DAY_MONTH = 30,										// days/month
		TIME_TICS_MONTH = TIME_TICS_DAY * TIME_DAY_MONTH,			// tics/month
		TIME_MONTH_YEAR = 10,										// months/year
		TIME_TICS_YEAR = TIME_TICS_DAY * TIME_MONTH_YEAR;			// tics/year
	constexpr std::string_view
		TIME_FORMAT_SHORT = "{:02}:{:02}:{:02}",
		TIME_FORMAT = "{:.3} {:2} {:02}:{:02}:{:02}",
		TIME_FORMAT_LONG = "{} {:2} {}, {:02}:{:02}:{:02}",
		TIME_FORMAT_DATE = "{:.3} {:2} {}",
		TIME_FORMAT_DATE_LONG = "{} {:2} {}",
		TIME_MONTH_NAMES[TIME_MONTH_YEAR] =
		{
			"January", "February", "March", "April", "May", "June", "July", "October", "November", "December"
		};
	constexpr unsigned long
		WAIT_LENGTH = TIME_TICS_SECOND * 5;

	// MESSAGES
	constexpr unsigned long MESSAGE_AGE_STALE = TIME_TICS_SECOND * 10;
	constexpr unsigned long MESSAGE_AGE_OLD = TIME_TICS_SECOND * 50;
	constexpr std::string_view
		MSG_NO_ACTION_FOR_KEY	= "Unbound key '{}'",
		MSG_WAIT				= "You wait a moment.",
		MSG_PLAYER_DEATH		= "You fall...",
		MSG_DEATH				= "{} dies!";

	// STATS
	constexpr int STAT_BASE_VALUE = 5;
	constexpr int STAT_BASE_VISION_RADIUS = 8;
	constexpr int STAT_BASE_HEALTH = 10;
	constexpr int STAT_BASE_MELEE = 3;
	constexpr int STAT_BASE_PSYCHIC = 2;
	constexpr int STAT_BASE_MELEE_DEF = 2;
	constexpr int STAT_BASE_PSYCHIC_DEF = 2;
	constexpr int STAT_BASE_SEDUCTION = 10;
	constexpr int STAT_BASE_MOVE_SPEED = TIME_TICS_SECOND * 5;
	constexpr int STAT_MIN_MOVE_SPEED = TIME_TICS_SECOND / 2;
	// todo balance level curve
	constexpr int ExperienceForLevel(int level)
		{ if(level <= 0) return 50; return (level - 1) * 50; }
	constexpr int AttributePointsPerLevel(int level)
		{ if(level <= 0) return 0; return (int) floor(1 + (4.0 * log((level + 2) / 2))); }}