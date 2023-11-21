#pragma once

#include <libtcod.hpp>

namespace RYSIC
{
	constexpr TCOD_ColorRGB
			C_BLACK = {0, 0, 0},
			C_GRAY0 = {20, 20, 20},
			C_GRAY1 = {50, 50, 50},
			C_GRAY2 = {70, 70, 70},
			C_GRAY3 = {120, 120, 120},
			C_GRAY4 = {200, 200, 200},
			C_WHITE = {255, 255, 255},
			C_RED = {220, 50, 50},
			C_RED_LIGHT = {255, 80, 80},
			C_CERISOFT = {255, 100, 0};
	
	constexpr TCOD_ColorRGB CF_TONE(const TCOD_ColorRGB col, int tone_idx, int tone_cnt)
	{
		return
		{
			(uint8_t) (((double) col.r / tone_cnt) * tone_idx),
			(uint8_t) (((double) col.g / tone_cnt) * tone_idx),
			(uint8_t) (((double) col.b / tone_cnt) * tone_idx)
		};
	}
}