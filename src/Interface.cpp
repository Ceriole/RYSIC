#include "Interface.hpp"

#include "ScreenUtil.hpp"

namespace RYSIC::Interface
{

	tcod::Console CreateComponentCanvas(
		const unsigned int w, const unsigned int h, const std::optional<TCOD_ColorRGB> fg,
		const std::optional<TCOD_ColorRGB> bg)
	{
		tcod::Console subconsole = tcod::Console{(int) w, (int) h};
		tcod::print_rect(subconsole, {0, 0, 0, 0}, " ", fg, bg);
		return subconsole;
	}

	void Container::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(w == 0 || h == 0)
			return;
		
		tcod::Console subconsole = tcod::Console{(int) w, (int) h};
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		tcod::blit(console, subconsole, {x, y});
	}

	bool Container::handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos)
	{
		if(!enabled)
			return false;
		if(w == 0 || h == 0)
			return false;
		screen_pos[0] -= x;
		screen_pos[1] -= y;
		bool handled = false;
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
			{
				handled = (*comp)->handleEvent(event, screen_pos);
				if(handled)
					break;
			}
		screen_pos[0] += x;
		screen_pos[1] += y;
		return handled;
	}

	void Frame::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(w == 0 || h == 0)
			return;

		
		tcod::Console subconsole = CreateComponentCanvas(w, h, fg, bg);

		int fw = w, fh = h;										// save width and height as signed ints
		int ox = x, oy = y, ow = w, oh = h;						// save position, width and height

		switch (decoration)										// adjust position, width and height
		{														// to fit window frame/decor
		case BORDERED:		fw -= 2; fh -= 2;
							x += 1; y += 1;		break;
		case FOOTER:		fh -= 1;
		case HEADER:		y += 1;				break;
		case SIDEBAR_R:		fw -= 1;
		case SIDEBAR_L:		x += 1;				break;
		case NONE:								break;
		}							
		if(((fw >= 1 || fh >= 1) && decoration != NONE) 		// only render components if they will
			|| ((fw > 1 && fh > 1) && decoration != BORDERED)	// be visible with decoration (if set)
			|| (fw > 2 && fh > 2))
		{
			w = fw; h = fh;										// set size for rendering components
			Container::render(subconsole);						// render child components
		}
		x = ox, y = oy, w = ow; h = oh;							// restore position, width and height
		
																// draw frame rectangle
		ScreenUtil::draw_rect(subconsole, {0, 0, (int) w, (int) h}, frame_fg, frame_bg);
		if(!title.empty())										// draw frame title
			tcod::print(subconsole, {2, 0}, title, std::nullopt, std::nullopt);
		
		tcod::blit(console, subconsole, {x, y});				// blit canvas to console
	}

	void Window::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(w == 0 || h == 0)
			return;
		tcod::Console subconsole = CreateComponentCanvas(w, h, fg, bg);

		Frame::render(subconsole); 								// render frame

		for(auto comp = window_components.begin(); comp != window_components.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		
		tcod::blit(console, subconsole, {x, y});				// blit canvas to console
	}

	bool Window::handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos)
	{
		if(!enabled)
			return false;
		if(w == 0 || h == 0)
			return false;
		screen_pos[0] -= x;
		screen_pos[1] -= y;
		bool handled = false;
		for(auto comp = window_components.begin(); comp != window_components.end(); comp++)
			if((*comp)->enabled)
			{
				handled = (*comp)->handleEvent(event, screen_pos);
				if(handled)
					break;
			}
		if(!handled)
			for(auto comp = children.begin(); comp != children.end(); comp++)
				if((*comp)->enabled)
				{
					handled = (*comp)->handleEvent(event, screen_pos);
					if(handled)
						break;
				}
		screen_pos[0] += x;
		screen_pos[1] += y;
		return handled;
	}

	void Label::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		
		TCOD_alignment_t justify = TCOD_LEFT;
		switch (align)
		{
		case HA_LEFT:		justify = TCOD_LEFT;	break;
		case HA_CENTER:		justify = TCOD_CENTER;	break;
		case HA_RIGHT:		justify = TCOD_RIGHT;	break;
		}
		tcod::print(console, {x, y}, text, fg, bg, justify);	// print text (mm simple)
	}

	void Button::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(w == 0 || h == 0)
			return;
		
		std::optional<TCOD_ColorRGB> button_color;
		switch (state)
		{
		case IDLE: button_color = color; break;
		case HOVERED: button_color = color_hover; break;
		case PRESSED: button_color = color_pressed; break;
		}
		tcod::Console subconsole = CreateComponentCanvas(w, h, label_color, color);

		if(!label.empty())
		{
			int lx = x;
			int ly = y;
			TCOD_alignment_t justify = TCOD_LEFT;
			switch (h_align)
			{
			case HA_LEFT:		justify = TCOD_LEFT;						break;
			case HA_CENTER:		justify = TCOD_CENTER;	lx += (w - 1) / 2;	break;
			case HA_RIGHT:		justify = TCOD_RIGHT;	lx += (w - 1);		break;
			}
			switch (v_align)
			{
			case VA_TOP:							break;
			case VA_CENTER:		ly += (h - 1) / 2;	break;
			case VA_BOTTOM:		ly += (h - 1);		break;
			}
			tcod::print(subconsole, {lx, ly}, label, label_color, std::nullopt, justify);
		}

		tcod::blit(console, subconsole, {x, y});				// blit canvas to console
	}

	bool Button::handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos)
	{
		if(!enabled)
			return false;
		if(w == 0 || h == 0)
			return false;
		if(event.type == SDL_MOUSEMOTION)
		{
			int local_x = event.motion.x - screen_pos[0];
			int local_y = event.motion.y - screen_pos[1];

			if(ScreenUtil::isWithin(local_x, local_y, x, y, w, h))
			{
				state = HOVERED;
				return true;
			}
			else
				state = IDLE;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN)
		{
			int local_x = event.button.x - screen_pos[0];
			int local_y = event.button.y - screen_pos[1];

			if(ScreenUtil::isWithin(local_x, local_y, x, y, w, h))
			{
				state = PRESSED;
				return true;
			}
			else
				state = IDLE;
		}
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			if(state == PRESSED && on_pressed)
			{
				on_pressed(event.button.x, event.button.y);
				return true;
			}
			state = IDLE;
		}
		return false;
	}
}