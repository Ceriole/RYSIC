#include "Game.hpp"

namespace RYSIC
{

	Game::Game(TCOD_ContextParams params, int width, int height)
		: m_width(width), m_height(height) : m_gameState(GS_CSFT)
	{
		m_console = tcod::Console{width, height};
		params.tcod_version = TCOD_COMPILEDVERSION;
		params.console = m_console.get();
		params.window_title = "RYSIC";
		params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
		params.vsync = true;
		m_context = tcod::Context(params);
	}

	Game::~Game()
	{
		m_console.release();
		m_context.close();
	}

	int Game::run()
	{
		while(1)
		{
			TCOD_console_clear(m_console.get());
			
			m_context.present(m_console);

			SDL_Event event;
			SDL_WaitEvent(nullptr);
			while(SDL_PollEvent(&event))
			{
				m_context.convert_event_coordinates(event);
				switch(event.type) {
					case SDL_QUIT:
						return 0;
				}
			}
		}
		return -1;
	}

}