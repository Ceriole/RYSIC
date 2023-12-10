#pragma once

#include <SDL2/SDL.h>
#include <libtcod.hpp>

#include "screen/Interface.hpp"
#include "world/World.hpp"
#include "EventHandler.hpp"

namespace RYSIC
{
	class Game
	{
	private:
		static Game* s_instance;
		tcod::Context m_context;
		tcod::Console m_console;
		int m_width, m_height;
		Pos m_mouse;
		bool m_quit, m_fullscreen;
		int m_exitCode = 0;
		Ref<Interface::Window> m_window;
		Ref<Interface::Canvas> m_canvas;
		Ref<Interface::ProgressBar> m_hp_bar;
		tcod::Tileset m_tileset;

		World::World* m_world = nullptr;
		EventHandler* m_event_handler = nullptr;

	public:
		Game(TCOD_ContextParams params, int width, int height);
		~Game();
		int run();
		void quit(int code = 0);
		void set_fullscreen(bool fullscreen);
		void set_title(const std::string &title);
		World::World* world() const { return m_world; }
		World::Map* map() const { return m_world->map(); }

		void set_event_handler(EventHandler* handler) { m_event_handler = handler; };
		
		static Game* const Instance() { return s_instance; }
	private:
		void handle_events();
		void regenerate_map();

		void populate_window();

		static SDL_HitTestResult HitTestCallback(SDL_Window *win, const SDL_Point *area, void *data);
	};
}