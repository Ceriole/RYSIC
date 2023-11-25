#pragma once

#include <libtcod.hpp>

namespace RYSIC
{
	using Color = std::optional<TCOD_ColorRGB>;
	using ColorRGBA = std::optional<TCOD_ColorRGBA>;

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
	
	TCOD_ColorRGB GetColorFraction(const TCOD_ColorRGB col, int tone_idx, int tone_cnt);
	TCOD_ColorRGB DarkenColor(const TCOD_ColorRGB col);
	TCOD_ColorRGB LightenColor(const TCOD_ColorRGB col);
}