#pragma once

#include <libtcod.hpp>

#include "Types.hpp"

namespace RYSIC::Util::Screen
{

	enum PrintDirection
	{
		LEFT_TO_RIGHT = 0,
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP
	};

	void set_char(TCOD_Console &console, int x, int y, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void set_char(TCOD_Console &console, int x, int y, const Character& gfx);
	void draw_hline(TCOD_Console &console, int x, int y, int w, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void draw_vline(TCOD_Console &console, int x, int y, int h, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void draw_rect(TCOD_Console &console, const std::array<int, 4> &rect, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void fill_rect(TCOD_Console &console, const std::array<int, 4> &rect, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void fill(TCOD_Console &console, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void dir_print(TCOD_Console &console, std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);

}