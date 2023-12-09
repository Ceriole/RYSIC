#include "Interface.hpp"

#include "ScreenUtil.hpp"
#include "Game.hpp"
#include "Util.hpp"

namespace RYSIC::Interface
{

	const Pos Component::get_abs_pos() const
	{
		if(parent != nullptr)
			return parent->get_abs_pos_of_child(this);
		return {rect.x, rect.y};
	}

	void Container::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.w == 0 || rect.h == 0)
			return;

		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		tcod::blit(console, subconsole, {rect.x, rect.y});
	}

	bool Container::handle_event(SDL_Event &event, Pos &screen_pos)
	{
		if(!enabled)
			return false;
		if(rect.w == 0 || rect.h == 0)
			return false;
		screen_pos -= {rect.x, rect.y};
		bool handled = false;
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
			{
				handled = (*comp)->handle_event(event, screen_pos);
				if(handled)
					break;
			}
		screen_pos += {rect.x, rect.y};
		return handled;
	}

	const Pos Container::get_abs_pos_of_child(const Component* child) const
	{
		for(auto &comp : children)
			if(comp.get() == child)
				return {rect.x + child->rect.x, rect.y + child->rect.y};
		return {child->rect.x, child->rect.y};
	}

	bool Container::add(const Ref<Component> comp)
	{
		if(children.find(comp) != children.end())
			return false;
		comp->parent = this;
		comp->parentType = CONTAINER;
		children.insert(comp);
		return true;
	}

	bool Container::remove(const Ref<Component> comp)
	{
		if(children.find(comp) == children.end())
			return nullptr;
		comp->parent = nullptr;
		comp->parentType = NONE;
		children.erase(comp);
		return true;
	}

	void Container::remove_all()
	{
		std::set<Ref<Component>> tempChildren(children);
		for(auto &comp : tempChildren)
			remove(comp);
	}

	void Frame::render(TCOD_Console &console)
	{
		if(!enabled)													// it not enabled skip drawing
			return;
		if(rect.w == 0 || rect.h == 0)									// if width or height is 0 then skip drawing
			return;
		
		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);

		Rect temp_rect = rect;											// save position, width and height

		switch (decoration)												// adjust position, width and height
		{																// to fit window frame/decor
		case BORDERED:		rect.w -= 2, rect.h -= 2,
							rect.x += 1, rect.y += 1;	break;
		case HEADER:		rect.y += 1;
		case FOOTER:		rect.h -= 1;				break;
		case SIDEBAR_L:		rect.x += 1;
		case SIDEBAR_R:		rect.w -= 1;				break;
		default:
		case NONE:										break;
		}

		if(((rect.w >= 1 || rect.h >= 1) && decoration != NONE) 		// only render components if they will
			|| ((rect.w > 1 && rect.h > 1) && decoration != BORDERED)	// be visible with decoration (if set)
			|| (rect.w > 2 && rect.h > 2))
			Container::render(subconsole);								// render child components
		
		rect = temp_rect;												// restore position, width and height
		
		switch (decoration)												// draw frame rectangle
		{
		case BORDERED:
			Util::Screen::draw_rect(subconsole, {0, 0, rect.w, rect.h}, 0, frame_fg, frame_bg); break;
		case HEADER:
			Util::Screen::draw_hline(subconsole, 0, 0, 0, 0, frame_fg, frame_bg); break;
		case FOOTER:
			Util::Screen::draw_hline(subconsole, 0, rect.h - 1, 0, 0, frame_fg, frame_bg); break;
		case SIDEBAR_L:
			Util::Screen::draw_vline(subconsole, 0, 0, 0, 0, frame_fg, frame_bg); break;
		case SIDEBAR_R:
			Util::Screen::draw_vline(subconsole, rect.w - 1, 0, 0, 0, frame_fg, frame_bg); break;
		default:
		case NONE:
			break;
		}
		
		if(!title.empty())												// draw frame title
			tcod::print_rect(subconsole, {MIN(rect.w, 2), 0, MAX(rect.w - 4, 0), 1}, title, std::nullopt, std::nullopt,
			alignmentToTCOD(title_align));

		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	const Pos Frame::get_abs_pos_of_child(const Component *child) const
	{
		Pos pos = Container::get_abs_pos_of_child(child);
		switch (decoration)
		{
		case BORDERED:		pos.x += 1, pos.y += 1;	break;
		case HEADER:		pos.y += 1;
		case FOOTER:		break;
		case SIDEBAR_L:		pos.x += 1;
		case SIDEBAR_R:		break;
		case NONE:			break;
		}
		return pos;
	}

	void Window::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.w == 0 || rect.h == 0)
			return;
		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);

		Frame::render(subconsole);								// render frame
		if(drawTopbar)
			Util::Screen::draw_hline(subconsole, 0, 0, 0, 0, topbar_fg, topbar_bg);
		for(auto comp = widgets.begin(); comp != widgets.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		
		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	bool Window::handle_event(SDL_Event &event, Pos &screen_pos)
	{
		if(!enabled)
			return false;
		if(rect.w == 0 || rect.h == 0)
			return false;
		bool handled = false;
		for(auto comp = widgets.begin(); comp != widgets.end(); comp++)
			if((*comp)->enabled)
			{
				handled = (*comp)->handle_event(event, screen_pos);
				if(handled)
					break;
			}
		if(!handled)
			for(auto comp = children.begin(); comp != children.end(); comp++)
				if((*comp)->enabled)
				{
					handled = (*comp)->handle_event(event, screen_pos);
					if(handled)
						break;
				}
		return handled;
	}

	const Pos Window::get_abs_pos_of_child(const Component *child) const
	{
		for(auto &comp : widgets)
			if(comp.get() == child)
				return Container::get_abs_pos_of_child(child);
		return Frame::get_abs_pos_of_child(child);
	}

	bool Window::add_widget(Ref<Component> comp)
	{
		if(widgets.find(comp) != widgets.end())
			return false;
		comp->parent = this;
		comp->parentType = WIDGET;
		widgets.insert(comp);
		return true;
	}

	bool Window::remove_widget(Ref<Component> comp)
	{
		if(widgets.find(comp) == widgets.end())
			return false;
		comp->parent = nullptr;
		comp->parentType = ParentType::NONE;
		widgets.erase(comp);
		return true;
	}

	void Window::remove_all_widgets()
	{
		for(auto &comp : widgets)
			remove(comp);
	}

	SDL_HitTestResult Window::get_hittest_result(const Pos &mouse)
	{
		const Pos local = mouse - get_abs_pos();
		for(auto &comp : widgets)
		{
			Button* btn = dynamic_cast<Button*>(comp.get());
			if(btn != nullptr)
			{
				if(Util::isWithin(local.x, local.y, btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h))
					return btn->hitTestResult;
			}
		}
		if(drawTopbar && Util::isWithin(local.x, local.y, 0, 0, rect.w, 1))
			return SDL_HITTEST_DRAGGABLE;
		return hitTestResult;
	}

	void Label::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		tcod::print(console, {rect.x, rect.y}, text, fg, bg, alignmentToTCOD(align));	// print text (mm simple)
	}

	void Button::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.w == 0 || rect.h == 0)
			return;

		std::optional<TCOD_ColorRGB> button_color;
		switch (state)
		{
		case IDLE: button_color = color; break;
		case HOVERED: button_color = color_hover; break;
		case PRESSED: button_color = color_pressed; break;
		}
		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, label_color, button_color);

		if(!label.empty())
		{
			int ly = 0;
			switch (v_align)
			{
			case VA_TOP:							break;
			case VA_CENTER:		ly += (rect.h - 1) / 2;	break;
			case VA_BOTTOM:		ly += (rect.h - 1);		break;
			}
			tcod::print_rect(subconsole, {0, ly, rect.w, 1}, label, label_color, std::nullopt, alignmentToTCOD(h_align));
		}

		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	bool Button::handle_event(SDL_Event &event, Pos &screen_pos)
	{
		if(!enabled)
			return false;
		if(rect.w == 0 || rect.h == 0)
			return false;
		if(event.type == SDL_MOUSEMOTION)
		{
			Pos local = Pos{event.motion.x, event.motion.y} - screen_pos;
			state = Util::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h) ? HOVERED : IDLE;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			Pos local = Pos{event.motion.x, event.motion.y} - screen_pos;

			state = Util::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h) ? HOVERED : IDLE;
			if(Util::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h))
			{
				if(on_pressed)
					on_pressed(event.button.x, event.button.y, this);
				state = PRESSED;
				return true;
			}
		}
		if(event.type == SDL_MOUSEBUTTONUP)
			state = IDLE;
		return false;
	}

	Canvas::Canvas(const Rect &_rect, Color _fg, Color _bg)
			: Container(_rect), canvas{_rect.w, _rect.h}
	{ 
		fg = _fg, bg = _bg;
		clear();
	}

	void Canvas::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.w == 0 || rect.h == 0)
			return;
		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);
		tcod::blit(subconsole, canvas);
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		tcod::blit(console, subconsole, {rect.x, rect.y});
	}

	void Canvas::clear()
	{
		Util::Screen::fill(canvas, ' ', fg, bg);
	}
	
	bool TypingLabel::start()
	{
		if(!enabled)
			return false;
		if(started || completed)
			return false;
		started = true;
		start_time = SDL_GetTicks64();
		return true;
	}

	bool TypingLabel::reset()
	{
		if(!started && !completed)
			return false;
		started = completed = false;
		current_text.clear();
		start_time = 0;
		return true;
	}

	bool TypingLabel::show()
	{
		if(completed)
			return false;
		completed = true;
		current_text = text;
		return true;
	}

	void TypingLabel::render(TCOD_Console &console)
	{
		if(!started || !enabled)
			return;
		if(text.empty())
			return;
		if(!completed)
		{
			unsigned int curr_len = (unsigned int) ((SDL_GetTicks64() - start_time) / delay);
			if(curr_len >= text.size())
				show();
			else
				current_text = text.substr(0, curr_len);
		}
		if(!current_text.empty())
			tcod::print_rect(console, {rect.x, rect.y, rect.w, rect.h}, current_text, fg, bg);
	}

	void ProgressBar::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		
		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, std::nullopt, bg);

		switch (direction)
		{
		default:
		case HORIZONTAL:
			Util::Screen::fill_rect(subconsole, {0, 0, (int) round(rect.w * value), rect.h}, 0, bg, fg); break;
		case VERTICAL:
			Util::Screen::fill_rect(subconsole, {0, 0, rect.w, (int) round(rect.h * value)}, 0, bg, fg); break;
		}
		
		tcod::print_rect(subconsole, {0, 0, 0, 0}, text, label_color, std::nullopt, alignmentToTCOD(align));

		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	void LogContainer::render(TCOD_Console &console)
	{
		if(!enabled)
			return;

		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);

		if(m_log)
		{
		tcod::Console message_box = Util::Screen::CreateComponentCanvas(rect.w, rect.h - 1, fg, bg);
			int y = rect.h - 1;
			for(auto msg = m_log->end(); msg != m_log->begin();)
			{
				--msg;
				y -= tcod::get_height_rect(rect.w, msg->str());
				tcod::print_rect(message_box, {0, y, rect.w, 0}, msg->str(), msg->fg_color(),  msg->bg_color());
				if(y < 1)
					break;
			}
			tcod::blit(subconsole, message_box, {0, 1});
		}
		else
		{
			tcod::print_rect(subconsole, {0, rect.h / 2, rect.w, 0}, "Null.", std::nullopt, std::nullopt, TCOD_CENTER);
		}
		

		Util::Screen::draw_hline(subconsole, 0, 0, 0, 0x2550, std::nullopt, std::nullopt);
		tcod::print_rect(subconsole, {0, 0, rect.w, 1}, "\u2561MESSAGE LOG\u255e", std::nullopt, std::nullopt, TCOD_CENTER);

		tcod::blit(console, subconsole, {rect.x, rect.y});
	}
}