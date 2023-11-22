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

	constexpr TCOD_ColorRGB CF_DARKEN(const TCOD_ColorRGB col)
	{
		return
		{
			(uint8_t) (col.r / 2),
			(uint8_t) (col.g / 2),
			(uint8_t) (col.b / 2)
		};
	}

	constexpr TCOD_ColorRGB CF_LIGHTEN(const TCOD_ColorRGB col)
	{
		return
		{
			(uint8_t) (MAX(((int) col.r * 2), UINT8_MAX)),
			(uint8_t) (MAX(((int) col.g * 2), UINT8_MAX)),
			(uint8_t) (MAX(((int) col.b * 2), UINT8_MAX))
		};
	}
}