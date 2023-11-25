#include "ScreenUtil.hpp"

#include <vector>

#include <SDL2/SDL.h>

namespace RYSIC::ScreenUtil
{

	void set_char(TCOD_Console &console, int x, int y, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		if(x < 0 || y < 0 || x >= console.w || y >= console.h)
			return;
		auto &tile = console.at(x, y);
		if(ch != 0) tile.ch = ch;
		if(fg.has_value()) tile.fg = fg.value();
		if(bg.has_value()) tile.bg = bg.value();
	}

	void draw_hline(TCOD_Console &console, int x, int y, int w, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		if(w == 0)
			x = 0, w = console.w;
		for(int cx = x; cx < x + w; cx++)
			set_char(console, cx, y, ch, fg, bg);
	}

	void draw_vline(TCOD_Console &console, int x, int y, int h, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		if(h == 0)
			y = 0, h = console.h;
		for(int cy = y; cy < y + h; cy++)
			set_char(console, x, cy, ch, fg, bg);
	}

	void draw_rect(TCOD_Console &console, const std::array<int, 4> &rect, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		int x = rect[0], y = rect[1], w = rect[2], h = rect[3];
		if(w == 0)
			x = 0, w = console.w;
		if(h == 0)
			y = 0, h = console.h;
		draw_hline(console, x, y, w, ch, fg, bg);
		draw_hline(console, x, y + h - 1, w, ch, fg, bg);
		
		draw_vline(console, x, y, h, ch, fg, bg);
		draw_vline(console, x + w - 1, y, h, ch, fg, bg);
	}

	void fill_rect(TCOD_Console &console, const std::array<int, 4> &rect, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		if(rect[2] == 0 && rect[3] == 0)
			fill(console, ch, fg, bg);
		else
			for(int cy = rect[1]; cy < rect[1] + rect[3]; cy++)
				draw_hline(console, rect[0], cy, rect[2], ch, fg, bg);
	}

	void fill(TCOD_Console &console, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		for(int x = 0; x < console.w; x++)
			for(int y = 0; y < console.h; y++)
				set_char(console, x, y, ch, fg, bg);
	}

	void directionalPrint(TCOD_Console &console, std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		for(int i = 0; i < text.size(); i++)
		{
			tcod::print(console, {x, y}, tcod::stringf("%c", text[i]), fg, bg);
			switch (dir)
			{
			default:
			case PrintDirection::LEFT_TO_RIGHT: x++; break;
			case PrintDirection::RIGHT_TO_LEFT: x--; break;
			case PrintDirection::TOP_TO_BOTTOM: y++; break;
			case PrintDirection::BOTTOM_TO_TOP: y--; break;
			}
		}
	}

	static std::vector<TypewriterText> s_typewriterTexts;
	void typewriterPrint(std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, unsigned int delay)
	{
		s_typewriterTexts.push_back({text, x, y, dir, fg, bg, SDL_GetTicks64(), delay, true});
	}

	std::vector<TypewriterText> typewriterShow(TCOD_Console &console)
	{
		unsigned long long curr_time = SDL_GetTicks64();
		std::vector<TypewriterText> completed;
		for(auto ttext = s_typewriterTexts.begin(); ttext != s_typewriterTexts.end(); ttext++)
		{
			if(ttext->active)
			{
				size_t curr_length = (curr_time - ttext->startTime) / ttext->delay;
				if(curr_length > ttext->text.size())
				{
					curr_length = ttext->text.size();
					completed.push_back(*ttext);
					ttext->active = false;
				}
				std::string to_print = ttext->text.substr(0, curr_length);
				directionalPrint(console, to_print, ttext->x, ttext->y, ttext->dir, ttext->fg, ttext->bg);
			}
		}
		auto ttext_completed_predicate = [](auto ttext) -> bool { return !ttext.active; };
		s_typewriterTexts.erase(std::remove_if(s_typewriterTexts.begin(), s_typewriterTexts.end(), ttext_completed_predicate), s_typewriterTexts.end());

		return completed;
	}

	void typewriterClear()
	{
		s_typewriterTexts.clear();
	}

}