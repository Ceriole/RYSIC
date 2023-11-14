#include "Game.hpp"

namespace RYSIC
{

	Game::Game(TCOD_ContextParams params, int width, int height)
		: m_width(width), m_height(height), m_gameState(GS_CSFT)
	{
		auto tileset = tcod::load_tilesheet("Andux_cp866ish.png", {16, 16}, tcod::CHARMAP_CP437);

		m_console = tcod::Console{width, height};
		params.tcod_version = TCOD_COMPILEDVERSION;
		params.console = m_console.get();
		params.tileset = tileset.get();
		params.window_title = "RYSIC";
		params.sdl_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS;
		params.vsync = true;
		params.pixel_width = tileset.get_tile_width() * m_console.get_width() * 2;
		params.pixel_height = tileset.get_tile_height() * m_console.get_height() * 2;
		m_context = tcod::Context(params);
		m_mouse = {0, 0};
		m_quit = false;
		m_fullscreen = false;

		SDL_SetWindowHitTest(m_context.get_sdl_window(), Game::hitTestCallback, this);
		SDL_SetWindowResizable(m_context.get_sdl_window(), SDL_TRUE);
	}

	Game::~Game()
	{
		m_console.release();
		m_context.close();
	}

	int Game::run()
	{
		while(!m_quit)
		{
			m_context.present(m_console); // update console to screen
			TCOD_console_clear(m_console.get()); // Clear console
			handleEvents();
			drawWindow();
		}
		return -1;
	}

	void Game::handleEvents()
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			m_context.convert_event_coordinates(event); // convert pixel (screen space) to tile space
			switch(event.type) {
				case SDL_KEYDOWN:
					break;
				case SDL_MOUSEMOTION:
					m_mouse.x = event.motion.x;
					m_mouse.y = event.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						m_mouse.x = event.button.x;
						m_mouse.y = event.button.y;
						if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 4, 0, 3, 1))
							m_quit = true;
						if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 8, 0, 3, 1))
							if(m_fullscreen)
							{
								SDL_SetWindowFullscreen(m_context.get_sdl_window(), 0);
								m_fullscreen = false;
							}
							else
							{
								SDL_SetWindowFullscreen(m_context.get_sdl_window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
								m_fullscreen = true;
							}
					}
					break;
				case SDL_QUIT:
					m_quit = true;
				default: break;
			}
		}
	}

	void Game::drawWindow()
	{
		constexpr std::array<TCOD_ColorRGB, 3> QUITBUTTON_COLORS = {TCOD_ColorRGB{0, 0, 0}, TCOD_ColorRGB{0, 0, 0}, TCOD_ColorRGB{0, 0, 0}};
		tcod::draw_rect(m_console, {0, 0, (int) m_width, (int) m_height}, 0, {{20, 20, 20}}, {{120, 120, 120}});
		tcod::draw_rect(m_console, {1, 1, (int) m_width - 2, (int) m_height - 2}, 0, {{200, 200, 200}}, {{20, 20, 20}});
		if(!m_fullscreen)
			tcod::draw_rect(m_console, {0, 0, (int) m_width, 1}, 0, {{20, 20, 20}}, {{200, 200, 200}});

		tcod::print(m_console, {1, 0}, SDL_GetWindowTitle(m_context.get_sdl_window()), std::nullopt, std::nullopt);

		// RESIZE HANDLE
		if(IS_AT(m_mouse.x, m_mouse.y, (int) m_width - 1, (int) m_height - 1))
			tcod::print(m_console, {(int) m_width - 1, (int) m_height - 1}, "\u255D", {{200, 200, 200}}, std::nullopt);
		else
			tcod::print(m_console, {(int) m_width - 1, (int) m_height - 1}, "\u255D", std::nullopt, std::nullopt);

		// SIZE BUTTON
		std::string sizeBtnText = m_fullscreen ? " \u25BC " : " \u25B2 ";
		if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 8, 0, 3, 1))
			tcod::print(m_console, {(int) m_width - 8, 0}, sizeBtnText, {{20,20,20}}, {{50, 50, 50}});
		else
			tcod::print(m_console, {(int) m_width - 8, 0}, sizeBtnText, {{20,20,20}}, {{70, 70, 70}});

		// CLOSE BUTTON
		if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 4, 0, 3, 1))
			tcod::print(m_console, {(int) m_width - 4, 0}, " X ", {{200,200,200}}, {{255, 80, 80}});
		else
			tcod::print(m_console, {(int) m_width - 4, 0}, " X ", {{200,200,200}}, {{220, 50, 50}});
	}

	SDL_HitTestResult Game::hitTestCallback(SDL_Window *win, const SDL_Point *area, void *data)
	{
		win;
		Game *game = (Game*) data;
		auto mouse = game->m_context.pixel_to_tile_coordinates(std::array<int, 2>{area->x, area->y});
		if(game->m_fullscreen)
			return SDL_HITTEST_NORMAL;
		if(mouse[1] == 0 &&														// if inside top bar
			!IS_WITHIN(mouse[0], mouse[1], (int) game->m_width - 4, 0, 3, 1) &&	// and NOT over close button
			!IS_WITHIN(mouse[0], mouse[1], (int) game->m_width - 8, 0, 3, 1)	// and NOT over size button
			)
			return SDL_HITTEST_DRAGGABLE;
		if(IS_AT(mouse[0], mouse[1], (int) game->m_width - 1, (int) game->m_height - 1))
			return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
		return SDL_HITTEST_NORMAL;
	}
}