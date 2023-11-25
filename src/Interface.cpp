#include "Interface.hpp"

#include "ScreenUtil.hpp"
#include "Game.hpp"

namespace RYSIC::Interface
{

	const Pos Component::getAbsolutePosition() const
	{
		if(parent != nullptr)
			return parent->getAbsolutePositionOfChild(this);
		return {rect.x, rect.y};
	}

	tcod::Console CreateComponentCanvas(
		const unsigned int w, const unsigned int h, const std::optional<TCOD_ColorRGB> fg,
		const std::optional<TCOD_ColorRGB> bg)
	{
		tcod::Console subconsole = tcod::Console{(int) w, (int) h};
		ScreenUtil::fill(subconsole, 0, fg, bg);
		return subconsole;
	}

	void Container::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.x == 0 || rect.y == 0)
			return;
		
		tcod::Console subconsole = CreateComponentCanvas(rect.w, rect.h, fg, bg);
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		tcod::blit(console, subconsole, {rect.x, rect.y});
	}

	bool Container::handleEvent(SDL_Event &event, Pos &screen_pos)
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
				handled = (*comp)->handleEvent(event, screen_pos);
				if(handled)
					break;
			}
		screen_pos += {rect.x, rect.y};
		return handled;
	}

	const Pos Container::getAbsolutePositionOfChild(const Component* child) const
	{
		for(auto &comp : children)
			if(comp.get() == child)
				return {rect.x + child->rect.x, rect.y + child->rect.y};
		return {child->rect.x, child->rect.y};
	}

	bool Container::Add(const Ref<Component> comp)
	{
		if(children.find(comp) != children.end())
			return false;
		comp->parent = this;
		children.insert(comp);
		return true;
	}

	bool Container::Remove(const Ref<Component> comp)
	{
		if(children.find(comp) == children.end())
			return nullptr;
		comp->parent = nullptr;
		comp->parentType = NONE;
		children.erase(comp);
		return true;
	}

	void Container::RemoveAll()
	{
		for(auto &comp : children)
			Remove(comp);
	}

	void Frame::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.w == 0 || rect.h == 0)
			return;
		
		tcod::Console subconsole = CreateComponentCanvas(rect.w, rect.h, fg, bg);

		Rect temp_rect = rect;										// save position, width and height

		switch (decoration)										// adjust position, width and height
		{														// to fit window frame/decor
		case BORDERED:		rect.w -= 2, rect.h -= 2,
							rect.x += 1, rect.y += 1;	break;
		case HEADER:		rect.y += 1;
		case FOOTER:		rect.h -= 1;				break;
		case SIDEBAR_L:		rect.x += 1;
		case SIDEBAR_R:		rect.w -= 1;				break;
		case NONE:										break;
		}							
		if(((rect.w >= 1 || rect.h >= 1) && decoration != NONE) 		// only render components if they will
			|| ((rect.w > 1 && rect.h > 1) && decoration != BORDERED)	// be visible with decoration (if set)
			|| (rect.w > 2 && rect.h > 2))
			Container::render(subconsole);						// render child components
		
		rect = temp_rect;										// restore position, width and height
		
																// draw frame rectangle
		ScreenUtil::draw_rect(subconsole, {0, 0, rect.w, rect.h}, 0, frame_fg, frame_bg);
		if(!title.empty())										// draw frame title
			tcod::print_rect(subconsole, {MIN(rect.w, 2), 0, MAX(rect.w - 4, 0), 1}, title, std::nullopt, std::nullopt, alignmentToTCOD(title_align));

		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	const Pos Frame::getAbsolutePositionOfChild(const Component *child) const
	{
		Pos pos = Container::getAbsolutePositionOfChild(child);
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
		tcod::Console subconsole = CreateComponentCanvas(rect.w, rect.h, fg, bg);

		Frame::render(subconsole);								// render frame
		if(drawTopbar)
			ScreenUtil::draw_hline(subconsole, 0, 0, 0, 0, topbar_fg, topbar_bg);
		for(auto comp = widgets.begin(); comp != widgets.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		
		tcod::blit(console, subconsole, {rect.x, rect.y});				// blit canvas to console
	}

	bool Window::handleEvent(SDL_Event &event, Pos &screen_pos)
	{
		if(!enabled)
			return false;
		if(rect.w == 0 || rect.h == 0)
			return false;
		bool handled = false;
		for(auto comp = widgets.begin(); comp != widgets.end(); comp++)
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
		return handled;
	}

	const Pos Window::getAbsolutePositionOfChild(const Component *child) const
	{
		for(auto &comp : widgets)
			if(comp.get() == child)
				return Container::getAbsolutePositionOfChild(child);
		return Frame::getAbsolutePositionOfChild(child);
	}

	bool Window::AddWidget(Ref<Component> comp)
	{
		if(widgets.find(comp) != widgets.end())
			return false;
		comp->parent = this;
		comp->parentType = WIDGET;
		widgets.insert(comp);
		return true;
	}

	bool Window::RemoveWidget(Ref<Component> comp)
	{
		if(widgets.find(comp) == widgets.end())
			return false;
		comp->parent = nullptr;
		comp->parentType = ParentType::NONE;
		widgets.erase(comp);
		return true;
	}

	void Window::RemoveAllWidgets()
	{
		for(auto &comp : widgets)
			Remove(comp);
	}

	SDL_HitTestResult Window::getHitTestResult(const Pos &mouse)
	{
		const Pos local = mouse - getAbsolutePosition();
		for(auto &comp : widgets)
		{
			Button* btn = dynamic_cast<Button*>(comp.get());
			if(btn != nullptr)
			{
				if(ScreenUtil::isWithin(local.x, local.y, btn->rect.x, btn->rect.y, btn->rect.w, btn->rect.h))
					return btn->hitTestResult;
			}
		}
		if(drawTopbar && ScreenUtil::isWithin(local.x, local.y, 0, 0, rect.w, 1))
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
		tcod::Console subconsole = CreateComponentCanvas(rect.w, rect.h, label_color, button_color);

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

	bool Button::handleEvent(SDL_Event &event, Pos &screen_pos)
	{
		if(!enabled)
			return false;
		if(rect.w == 0 || rect.h == 0)
			return false;
		if(event.type == SDL_MOUSEMOTION)
		{
			Pos local = Pos{event.motion.x, event.motion.y} - screen_pos;
			state = ScreenUtil::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h) ? HOVERED : IDLE;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			Pos local = Pos{event.motion.x, event.motion.y} - screen_pos;

			state = ScreenUtil::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h) ? HOVERED : IDLE;
			if(ScreenUtil::isWithin(local.x, local.y, rect.x, rect.y, rect.w, rect.h))
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
			: Container(_rect), console{_rect.w, _rect.h}
	{ 
		fg = _fg, bg = _bg;
		clear();
	}

	void Canvas::render(TCOD_Console &console)
	{
		if(!enabled)
			return;
		if(rect.x == 0 || rect.y == 0)
			return;
		
		tcod::Console subconsole = CreateComponentCanvas(rect.w, rect.h, fg, bg);
		tcod::blit(subconsole, this->console);
		for(auto comp = children.begin(); comp != children.end(); comp++)
			if((*comp)->enabled)
				(*comp)->render(subconsole);
		tcod::blit(console, subconsole, {rect.x, rect.y});
	}

	void Canvas::clear()
	{
		ScreenUtil::fill(console, ' ', fg, bg);
	}
}