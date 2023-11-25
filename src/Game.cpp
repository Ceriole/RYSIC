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

			m_window = CreateRef<Interface::Window>(Rect{0, 0, m_width, m_height}, C_GRAY4, C_GRAY3, C_GRAY3, C_GRAY4);
			auto btn_fullscreen = CreateRef<Interface::Button>(Rect{m_width - 9, 0, 3, 1}, "\u25b2", C_BLACK, C_GRAY2, C_GRAY3, C_GRAY0, [&](int, int, Interface::Button* const btn) {
				setFullscreen(!m_fullscreen);
				if(m_fullscreen)
					btn->label = "\u25bc";
				else
					btn->label = "\u25b2";
			});
			auto close_button = CreateRef<Interface::Button>(Rect{m_width - 5, 0, 3, 1}, "X", C_WHITE, C_RED, [&](int, int, Interface::Button* const) { quit(); });

			m_canvas = CreateRef<Interface::Canvas>(Rect{0, 0, m_window->rect.w - 2, m_window->rect.h - 2}, C_WHITE, C_BLACK);
			m_window->Add(m_canvas);
			m_window->AddWidget(close_button);
			m_window->AddWidget(btn_fullscreen);
			m_window->title = SDL_GetWindowTitle(m_context.get_sdl_window());
			SDL_SetWindowHitTest(m_context.get_sdl_window(), Game::HitTestCallback, this);
			setFullscreen(false);
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
			handleEvents();

			
			
			m_window->render(m_console);
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
		m_window->drawTopbar = !m_fullscreen;
		if(m_fullscreen)
			SDL_SetWindowFullscreen(m_context.get_sdl_window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen(m_context.get_sdl_window(), 0);
	}

	bool Game::isKeyDown(SDL_Keycode key)
	{
		return std::find(m_keys_down.begin(), m_keys_down.end(), key) != m_keys_down.end();
	}

	bool Game::isKeyPressed(SDL_Keycode key)
	{
		return std::find(m_keys_pressed.begin(), m_keys_pressed.end(), key) != m_keys_pressed.end();
	}

	void Game::handleEvents()
	{
		m_keys_pressed.clear();
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			m_context.convert_event_coordinates(event); // convert pixel (screen space) to tile space
			Pos eventWindowCoords;
			if(m_window->handleEvent(event, eventWindowCoords))
				continue;
			switch(event.type) {
				case SDL_KEYDOWN:
					m_keys_down.push_back(event.key.keysym.sym);
					m_keys_pressed.push_back(event.key.keysym.sym);
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
						/*if(ScreenUtil::isWithin(m_mouse.x, m_mouse.y, (int) m_width - 4, 0, 3, 1))
							quit();
						if(ScreenUtil::isWithin(m_mouse.x, m_mouse.y, (int) m_width - 8, 0, 3, 1))
							setFullscreen(!m_fullscreen);*/
					}
					break;
				case SDL_QUIT:
					quit();
				default: break;
			}
		}
	}

	
	SDL_HitTestResult Game::HitTestCallback(SDL_Window*, const SDL_Point *area, void *data)
	{
		Game *game = (Game*) data;
		auto mouse_arr = game->m_context.pixel_to_tile_coordinates(std::array<int, 2>{area->x, area->y});
		Pos mouse = {mouse_arr[0], mouse_arr[1]};
		return game->m_window->getHitTestResult(mouse);
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
			ScreenUtil::directionalPrint(m_console, "CERISOFT", (m_width - 8) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {GetColorFraction(C_CERISOFT, 4 - (m_gs_ctr - 3), 4)}, std::nullopt);
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
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 12:
		case 13:
		case 14:
		case 15:
			ScreenUtil::directionalPrint(m_console, "RYSIC", (m_width - 5) / 2, m_height / 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {GetColorFraction(C_WHITE, 4 - (m_gs_ctr - 12), 4)}, std::nullopt);
			ScreenUtil::directionalPrint(m_console, "PRESS ENTER", (m_width - 11) / 2, (m_height / 2) + 2, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {GetColorFraction(C_GRAY2, 4 - (m_gs_ctr - 12), 4)}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 100) 
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
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
		static int selected = 0;
		auto completed = ScreenUtil::typewriterShow(m_console);
		switch (m_gs_ctr)
		{
		case 0: // init
			ScreenUtil::typewriterPrint("RYSIC", 3, 3, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt, 20);
			m_gs_ctr++;
			break;
		case 1:
			if(completed.size() > 0)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 2:
			ScreenUtil::directionalPrint(m_console, "RYSIC", 3, 3, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			if(SDL_GetTicks64() - m_gs_timer > 1000)
			{
				m_gs_timer = SDL_GetTicks64(); m_gs_ctr++;
			}
			break;
		case 3:
			if(isKeyPressed(SDL_KeyCode::SDLK_UP) || isKeyPressed(SDL_KeyCode::SDLK_KP_8))
			{
				selected--;
				if(selected < 0)
					selected = 2;
			}
			if(isKeyPressed(SDL_KeyCode::SDLK_DOWN) || isKeyPressed(SDL_KeyCode::SDLK_KP_2))
			{
				selected++;
				if(selected > 2)
					selected = 0;
			}
			if(isKeyPressed(SDL_KeyCode::SDLK_RETURN))
				switch (selected)
				{
				case 0:
					/* code */
					break;
				
				default:
					break;
				}

			if(selected == 0)
				ScreenUtil::directionalPrint(m_console, "NEW GAME <", 3, 5, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			else
				ScreenUtil::directionalPrint(m_console, "NEW GAME", 3, 5, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_GRAY2}, std::nullopt);
			if(selected == 1)
				ScreenUtil::directionalPrint(m_console, "OPTION <", 3, 7, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			else
				ScreenUtil::directionalPrint(m_console, "OPTION", 3, 7, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_GRAY2}, std::nullopt);
			if(selected == 2)
				ScreenUtil::directionalPrint(m_console, "QUIT <", 3, 9, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_WHITE}, std::nullopt);
			else
				ScreenUtil::directionalPrint(m_console, "QUIT", 3, 9, ScreenUtil::PrintDirection::LEFT_TO_RIGHT, {C_GRAY2}, std::nullopt);
			break;
		}
	}
}