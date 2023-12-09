#pragma once

#include <SDL2/SDL.h>
#include <libtcod.hpp>

#include "screen/Interface.hpp"
#include "world/World.hpp"

namespace RYSIC
{

	class Game
	{
	private:
		tcod::Context m_context;
		tcod::Console m_console;
		int m_width, m_height;
		Pos m_mouse;
		bool m_quit, m_fullscreen;
		int m_exitCode = 0;
		Ref<Interface::Window> m_window;
		Ref<Interface::Canvas> m_canvas;
		tcod::Tileset m_tileset;

		World::World* m_world;

	public:
		Game(TCOD_ContextParams params, int width, int height);
		~Game();
		int run();
		void quit(int code = 0);
		void set_fullscreen(bool fullscreen);
	private:
		void handle_events();
		void set_title(const std::string &title);
		void regenerate_map();

		void populate_window();

		static SDL_HitTestResult HitTestCallback(SDL_Window *win, const SDL_Point *area, void *data);
	};
}