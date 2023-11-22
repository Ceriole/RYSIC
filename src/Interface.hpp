#pragma once

#include <memory>

#include <SDL.h>
#include <libtcod.hpp>

#include "Colors.hpp"

namespace RYSIC::Interface
{

	enum HorizontalAlignment { HA_LEFT, HA_CENTER, HA_RIGHT };
	enum VerticalAlignment { VA_TOP, VA_CENTER, VA_BOTTOM };

	struct Component
	{
		int x = 0, y = 0;
		bool enabled = true;

		Component() = default;
		Component(int _x, int _y)
			: x(_x), y(_y)
		{}
		~Component() = default;
		
		virtual void render(TCOD_Console&) {};
		virtual bool handleEvent(SDL_Event&, std::array<int, 2>&) { return false; };
	};

	struct Container : public Component
	{
		unsigned int w = 0, h = 0;
		std::vector<std::unique_ptr<Component>> children;
		std::optional<TCOD_ColorRGB> fg, bg;

		Container() = default;
		Container(int _x, int _y, unsigned int _w, unsigned int _h)
			: Component(_x, _y), w(_w), h(_h)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos) override;
	};

	struct Frame : public Container
	{
		std::optional<TCOD_ColorRGB> frame_fg, frame_bg;
		std::string title;

		enum FrameDecoration
		{
			NONE,
			BORDERED,
			HEADER,
			FOOTER,
			SIDEBAR_L,
			SIDEBAR_R
		} decoration;

		Frame() = default;
		Frame(int _x, int _y, unsigned int _w, unsigned int _h)
			: Container(_x, _y, _w, _h)
		{}
		Frame(int _x, int _y, unsigned int _w, unsigned int _h,
		std::optional<TCOD_ColorRGB> _frame_fg, std::optional<TCOD_ColorRGB> _frame_bg)
			: Container(_x, _y, _w, _h), frame_fg(_frame_fg), frame_bg(_frame_bg), decoration(BORDERED)
		{}

		void render(TCOD_Console &console) override;
	};

	struct Window : public Frame
	{
		std::optional<TCOD_ColorRGB> topbar_fg, topbar_bg;
		std::vector<std::unique_ptr<Component>> window_components;

		Window() = default;
		Window(int _x, int _y, unsigned int _w, unsigned int _h,
		std::optional<TCOD_ColorRGB> _frame_fg, std::optional<TCOD_ColorRGB> _frame_bg,
		std::optional<TCOD_ColorRGB> _topbar_fg, std::optional<TCOD_ColorRGB> _topbar_bg)
			: Frame(_x, _y, _w, _h, _frame_fg, _frame_bg), topbar_fg(_topbar_fg), topbar_bg(_topbar_bg)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos) override;
	};

	struct Label : public Component
	{
		std::string text;
		HorizontalAlignment align = HA_LEFT;
		std::optional<TCOD_ColorRGB> fg, bg;

		Label() = default;
		Label(int _x, int _y, std::string _text)
			: Component(_x, _y), text(_text)
		{}
		Label(int _x, int _y, std::string _text, std::optional<TCOD_ColorRGB> _fg, std::optional<TCOD_ColorRGB> _bg)
			: Component(_x, _y), text(_text), fg(_fg), bg(_bg)
		{}

		virtual void render(TCOD_Console &console) override;
	};

	struct Button : public Component
	{
		unsigned int w = 0, h = 0;
		std::string label;
		VerticalAlignment v_align = VA_CENTER;
		HorizontalAlignment h_align = HA_CENTER;
		std::optional<TCOD_ColorRGB> label_color, color, color_hover, color_pressed;
		std::function<void(int, int)> on_pressed;

		enum State { IDLE, HOVERED, PRESSED } state = IDLE;

		Button() = default;
		Button(int _x, int _y, unsigned int _w, unsigned int _h, std::string _label,
			std::optional<TCOD_ColorRGB> _label_color, std::optional<TCOD_ColorRGB> _color,
			std::optional<TCOD_ColorRGB> _color_hover, std::optional<TCOD_ColorRGB> _color_pressed,
			std::function<void(int, int)> _on_pressed)
			: Component(_x, _y), w(_w), h(_h), label(_label), label_color(_label_color), color(_color),
			color_hover(_color_hover), color_pressed(_color_pressed), on_pressed(_on_pressed)
		{}
		Button(int _x, int _y, unsigned int _w, unsigned int _h, std::string _label,
			std::optional<TCOD_ColorRGB> _label_color, TCOD_ColorRGB _color, std::function<void(int, int)> _on_pressed)
			: Button(_x, _y, _w, _h, _label, _label_color, {_color}, CF_LIGHTEN(_color), CF_DARKEN(_color), _on_pressed)
		{}

		virtual void render(TCOD_Console &console) override;
		virtual bool Button::handleEvent(SDL_Event &event, std::array<int, 2> &screen_pos) override;
	};
	
}