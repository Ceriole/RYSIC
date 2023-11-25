#pragma once

#include <SDL.h>
#include <libtcod.hpp>
#include <set>

#include "Pointers.hpp"
#include "Types.hpp"
#include "Colors.hpp"

namespace RYSIC::Interface
{

	enum HorizontalAlignment { HA_LEFT, HA_CENTER, HA_RIGHT };
	enum VerticalAlignment { VA_TOP, VA_CENTER, VA_BOTTOM };

	constexpr TCOD_alignment_t alignmentToTCOD(HorizontalAlignment align)
	{
		switch (align)
		{
		default:
		case HA_LEFT:		return TCOD_LEFT;
		case HA_CENTER:		return TCOD_CENTER;
		case HA_RIGHT:		return TCOD_RIGHT;
		}
	}

	class Container;
	
	class Component
	{
	protected:
		Container* parent = nullptr;
		enum ParentType { NONE, CONTAINER, WIDGET } parentType = NONE;

	public:
		Rect rect = {};
		bool enabled = true;
		SDL_HitTestResult hitTestResult = SDL_HITTEST_NORMAL;

	public:
		Component() = default;
		Component(const Pos& _pos)
			: rect{_pos.x, _pos.y, 0, 0}
		{}
		Component(const Rect& _rect)
			: rect(_rect)
		{}
		~Component() = default;
		
		virtual void render(TCOD_Console&) = 0;
		virtual bool handleEvent(SDL_Event&, Pos&) { return false; };

		const Container* getParent() const { return parent; };
		const Pos getAbsolutePosition() const;

		friend class Container;
		friend class Window;
	};

	class Container : public Component
	{
	protected:
		std::set<Ref<Component>> children;

	public:
		Color fg, bg;

		Container() = default;
		Container(Rect _rect)
			: Component(_rect)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool handleEvent(SDL_Event &event, Pos &screen_pos) override;
		virtual const Pos getAbsolutePositionOfChild(const Component* child) const;

		bool Add(const Ref<Component> comp);
		bool Remove(const Ref<Component> comp);
		void RemoveAll();
	};

	class Frame : public Container
	{
	public:
		Color frame_fg, frame_bg;
		std::string title;
		HorizontalAlignment title_align = HA_LEFT;
		enum FrameDecoration
		{
			NONE,
			BORDERED,
			HEADER,
			FOOTER,
			SIDEBAR_L,
			SIDEBAR_R
		} decoration;
	
	public:
		Frame() = default;
		Frame(const Rect &_rect)
		: Container(_rect)
		{}
		Frame(const Rect &_rect, Color _frame_fg, Color _frame_bg)
			: Container(_rect), frame_fg(_frame_fg), frame_bg(_frame_bg), decoration(BORDERED)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual const Pos getAbsolutePositionOfChild(const Component* child) const override;
	};

	class Window : public Frame
	{
	protected:
		std::set<Ref<Component>> widgets;

	public:
		bool drawTopbar = true;
		Color topbar_fg, topbar_bg;

	public:
		Window() = default;
		Window(const Rect &_rect, Color _frame_fg, Color _frame_bg, Color _topbar_fg, Color _topbar_bg)
			: Frame(_rect, _frame_fg, _frame_bg), topbar_fg(_topbar_fg), topbar_bg(_topbar_bg)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool handleEvent(SDL_Event &event, Pos &screen_pos) override;

		virtual const Pos getAbsolutePositionOfChild(const Component* child) const override;

		bool AddWidget(const Ref<Component> comp);
		bool RemoveWidget(const Ref<Component> comp);
		void RemoveAllWidgets();

		SDL_HitTestResult getHitTestResult(const Pos& mouse);
	};

	class Label : public Component
	{
	public:
		std::string text;
		HorizontalAlignment align = HA_LEFT;
		Color fg, bg;

	public:
		Label() = default;
		Label(const Pos &_pos, std::string _text)
			: Component(_pos), text(_text)
		{}
		Label(const Pos &_pos, std::string _text, Color _fg, Color _bg)
			: Component(_pos), text(_text), fg(_fg), bg(_bg)
		{}

		virtual void render(TCOD_Console &console) override;
	};

	class Button : public Component
	{
	public:
		std::string label;
		VerticalAlignment v_align = VA_CENTER;
		HorizontalAlignment h_align = HA_CENTER;
		Color label_color, color, color_hover, color_pressed;
		std::function<void(int, int, Button* const)> on_pressed;
		enum State { IDLE, HOVERED, PRESSED } state = IDLE;
	
	public:
		Button() = default;
		Button(const Rect &_rect, std::string _label,
			Color _label_color, Color _color, Color _color_hover, Color _color_pressed,
			std::function<void(int, int, Button* const)> _on_pressed)
			: Component(_rect), label(_label), label_color(_label_color), color(_color),
			color_hover(_color_hover), color_pressed(_color_pressed), on_pressed(_on_pressed)
		{}
		Button(const Rect &_rect, std::string _label,
			Color _label_color, TCOD_ColorRGB _color, std::function<void(int, int, Button* const)> _on_pressed)
			: Button(_rect, _label, _label_color, _color, LightenColor(_color), DarkenColor(_color), _on_pressed)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool Button::handleEvent(SDL_Event &event, Pos &screen_pos) override;
	};

	class Canvas : public Container
	{
	public:
		tcod::Console console;

	public:
		Canvas() = default;
		Canvas(const Rect &_rect, Color _fg, Color _bg);

		virtual void render(TCOD_Console &console) override;
		void clear();
	};
	
}