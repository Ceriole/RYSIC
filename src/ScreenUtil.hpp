#pragma once

#include <libtcod.hpp>

namespace RYSIC::ScreenUtil
{
	constexpr size_t TYPEWRITER_MAX_TEXTS = 32U;

	constexpr bool isWithin(int x, int y, int ax, int ay, int aw, int ah)
		{ return x >= ax && x < ax + aw && y >= ay && y < ay + ah; }
	constexpr bool isAt(int x, int y, int px, int py)
		{ return x == px && y == py; }

	enum PrintDirection
	{
		LEFT_TO_RIGHT = 0,
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP
	};

	struct TypewriterText
	{
		std::string text;
		int x, y;
		PrintDirection dir;
		std::optional<TCOD_ColorRGB> fg, bg;
		unsigned long long startTime, delay;
		bool active = false;
	};

	void draw_rect(TCOD_Console &console, std::array<int, 4> rect, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void directionalPrint(TCOD_Console &console, std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg);
	void typewriterPrint(std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, unsigned int delay);
	std::vector<TypewriterText> typewriterShow(TCOD_Console &console);
	void typewriterClear();

}