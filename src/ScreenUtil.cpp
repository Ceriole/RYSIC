#include "ScreenUtil.hpp"

#include <vector>

#include <SDL2/SDL.h>

namespace RYSIC::ScreenUtil
{

	void draw_rect(TCOD_Console &console, std::array<int, 4> rect, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		for(int x = rect[0]; x < (rect[0] + rect[2]); x++)
		{
			tcod::print(console, {x, rect[1]}, " ", fg, bg);
			tcod::print(console, {x, rect[1] + rect[3] - 1}, " ", fg, bg);
		}

		for(int y = rect[1]; y < (rect[1] + rect[3]); y++)
		{
			tcod::print(console, {rect[0], y}, " ", fg, bg);
			tcod::print(console, {rect[0] + rect[2] - 1, y}, " ", fg, bg);
		}
	}

	void directionalPrint(TCOD_Console &console, std::string text, int x, int y, PrintDirection dir, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg)
	{
		for(int i = 0; i < text.size(); i++)
		{
			/*
			// setting values indvidually
			TCOD_console_put_char(&console, x, y, cch[0], TCOD_BKGND_NONE);
			if(bg.has_value())
				TCOD_console_set_char_background(&console, x, y, bg.value(), TCOD_BKGND_SET);
			if(fg.has_value())
				TCOD_console_set_char_foreground(&console, x, y, fg.value());
			*/
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