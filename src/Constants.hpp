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
	constexpr std::array<int, 16*3>
		CHARMAP_SPRITES = {
			0x3000, 0x3001, 0x3002, 0x3003, 0x3004, 0x3005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x3100, 0x3101, 0x3102, 0x3103, 0x3104, 0x3105, 0x3106, 0x3107, 0x3108, 0x3109, 0x310A, 0x310B, 0x310C, 0x0000, 0x0000, 0x0000,
			0x3200, 0x3201, 0x3202, 0x3203, 0x3204, 0x3205, 0x3206, 0x3207, 0x3208, 0x3209, 0x320A, 0x320B, 0x320C, 0x320D, 0x0000, 0x0000
		};
	static const auto
		CHARMAP = RYSIC::Util::join(tcod::CHARMAP_CP437, CHARMAP_SPRITES);

	// SCREEN
	constexpr unsigned int
		DEFAULT_WIDTH = 60,
		DEFAULT_HEIGHT = 35,
		DEFAULT_PIXEL_SIZE = 1;
}