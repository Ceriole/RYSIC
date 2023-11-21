#include "Game.hpp"
#include "ScreenUtil.hpp"

#include <iostream>

namespace RYSIC
{

	Game::Game(TCOD_ContextParams params, int width, int height)
		: m_width(width), m_height(height), m_gameState(GS_CSFT)
	{
		try
		{
			auto tileset = tcod::load_tilesheet("res/Andux_cp866ish.png", {16, 16}, tcod::CHARMAP_CP437);

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
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
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
			tcod::draw_rect(m_console, {1, 1, (int) m_width - 2, (int) m_height - 2}, 0, C_GRAY4, C_GRAY0);
			handleEvents();

			doState();

			drawWindow(); // draw border & buttons over everything else
		}
		return m_exitCode;
	}

	void Game::quit(int code)
	{
		printf("Quitting...\n");
		m_exitCode = code;
		m_quit = true;
	}

	void Game::setFullscreen(bool fullscreen)
	{
		if(m_fullscreen == fullscreen)
			return;
		m_fullscreen = fullscreen;
		if(m_fullscreen)
			SDL_SetWindowFullscreen(m_context.get_sdl_window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen(m_context.get_sdl_window(), 0);
	}

	bool Game::isKeyDown(SDL_Keycode key)
	{
		return std::find(m_keys_down.begin(), m_keys_down.end(), key) != m_keys_down.end();
	}

	void Game::handleEvents()
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			m_context.convert_event_coordinates(event); // convert pixel (screen space) to tile space
			switch(event.type) {
				case SDL_KEYDOWN:
					m_keys_down.push_back(event.key.keysym.sym);
					break;
				case SDL_KEYUP:
					{
						auto it = std::find(m_keys_down.begin(), m_keys_down.end(), event.key.keysym.sym);
						m_keys_down.erase(it);
					}
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
							quit();
						if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 8, 0, 3, 1))
							setFullscreen(!m_fullscreen);
					}
					break;
				case SDL_QUIT:
					quit();
				default: break;
			}
		}
	}

	void Game::doState()
	{
		switch (m_gameState)
		{
		case GS_CSFT: // cerisoft logo
			doStateCSFT(); break;
		case GS_TITL:
			doStateTITL(); break;
		}
	}

	void Game::drawWindow()
	{
		// BORDER
		ScreenUtil::draw_rect(m_console, {0, 0, (int) m_width, (int) m_height}, C_GRAY0, C_GRAY3);
		
		// TOP BAR + TITLE
		if(!m_fullscreen)
		{
			tcod::draw_rect(m_console, {0, 0, (int) m_width, 1}, 0, C_GRAY0, C_GRAY4);
			tcod::print(m_console, {1, 0}, SDL_GetWindowTitle(m_context.get_sdl_window()), C_GRAY3, std::nullopt);
		}
		else
			tcod::print(m_console, {1, 0}, SDL_GetWindowTitle(m_context.get_sdl_window()), C_GRAY2, std::nullopt);

		// RESIZE HANDLE
		if(!m_fullscreen)
		{
			if(IS_AT(m_mouse.x, m_mouse.y, (int) m_width - 1, (int) m_height - 1))
				tcod::print(m_console, {(int) m_width - 1, (int) m_height - 1}, "\u255D", C_GRAY4, std::nullopt);
			else
				tcod::print(m_console, {(int) m_width - 1, (int) m_height - 1}, "\u255D", std::nullopt, std::nullopt);
		}

		// SIZE BUTTON
		std::string sizeBtnText = m_fullscreen ? " \u25BC " : " \u25B2 ";
		if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 8, 0, 3, 1))
			tcod::print(m_console, {(int) m_width - 8, 0}, sizeBtnText, C_GRAY0, C_GRAY1);
		else
			tcod::print(m_console, {(int) m_width - 8, 0}, sizeBtnText, C_GRAY0, C_GRAY2);

		// CLOSE BUTTON
		if(IS_WITHIN(m_mouse.x, m_mouse.y, (int) m_width - 4, 0, 3, 1))
			tcod::print(m_console, {(int) m_width - 4, 0}, " X ", C_GRAY4, C_RED_LIGHT);
		else
			tcod::print(m_console, {(int) m_width - 4, 0}, " X ", C_GRAY4, C_RED);
	}

	SDL_HitTestResult Game::hitTestCallback(SDL_Window *win, const SDL_Point *area, void *data)
	{
		win; // unused param
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

	void Game::doStateCSFT()
	{
		auto completed = ScreenUtil::typewriterShow(m_console);
		switch (m_gs_ctr)
		{
		case 0: // init
			ScreenUtil::typewriterPrint("CERISOFT", (m_width - 8) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_CERISOFT}, std::nullopt, 100);
			m_gs_ctr++;
			break;
		case 1:
			if(completed.size() > 0)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 2:
			ScreenUtil::directionalPrint(m_console, "CERISOFT", (m_width - 8) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_CERISOFT}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 1000)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			ScreenUtil::directionalPrint(m_console, "CERISOFT", (m_width - 8) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {CF_TONE(C_CERISOFT, 4 - (m_gs_ctr - 3), 4)}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 100)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
		case 7:
			if(SDL_GetTicks64() - m_gs_timer > 1000)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 8:
			ScreenUtil::typewriterPrint("RYSIC", (m_width - 5) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt, 100);
			m_gs_ctr++;
			break;
		case 9:
			if(completed.size() > 0)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 10:
			ScreenUtil::directionalPrint(m_console, "RYSIC", (m_width - 5) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 1000)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 11:
			ScreenUtil::directionalPrint(m_console, "RYSIC", (m_width - 5) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			ScreenUtil::directionalPrint(m_console, "PRESS ENTER", (m_width - 11) / 2, (m_height / 2) + 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_GRAY2}, std::nullopt);
			if(isKeyDown(SDL_KeyCode::SDLK_RETURN))
				m_gs_ctr++;
			break;
		case 12:
		case 13:
		case 14:
		case 15:
			ScreenUtil::directionalPrint(m_console, "RYSIC", (m_width - 5) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {CF_TONE(C_WHITE, 4 - (m_gs_ctr - 12), 4)}, std::nullopt);
			ScreenUtil::directionalPrint(m_console, "PRESS ENTER", (m_width - 11) / 2, (m_height / 2) + 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {CF_TONE(C_GRAY2, 4 - (m_gs_ctr - 12), 4)}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 100)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
		case 16:
			m_gs_ctr = 0;
			m_gameState = GS_TITL;
			break;
		default:
			break;
		}
	}
	
	void Game::doStateTITL()
	{
		tcod::print(m_console, {3, 3}, "TODO", {C_RED_LIGHT}, std::nullopt);
	}
}