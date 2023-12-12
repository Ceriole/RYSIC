#include "Game.hpp"

#include <iostream>
#include <fmt/core.h>

#include "Constants.hpp"
#include "screen/ScreenUtil.hpp"
#include "world/MapGenerator.hpp"
#include "world/entity/StaticEntities.hpp"
#include "world/entity/Actor.hpp"
#include "Version.h"

namespace RYSIC
{

	Game* Game::s_instance = nullptr;

	Game::Game(TCOD_ContextParams params, int width, int height)
		: m_width(width), m_height(height)
	{
		s_instance = this;
		try
		{
			m_tileset = tcod::load_tilesheet(Constants::DEFAULT_TILESET_FILE, Constants::TILESET_COLUMNS_ROWS, Constants::CHARMAP);
			m_console = tcod::Console{width, height};
			params.tcod_version = TCOD_COMPILEDVERSION;
			params.console = m_console.get();
			params.tileset = m_tileset.get();
			params.window_title = "nul";
			params.sdl_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN;
			params.vsync = true;
			params.pixel_width = m_tileset.get_tile_width() * m_console.get_width() * Constants::DEFAULT_PIXEL_SIZE;
			params.pixel_height = m_tileset.get_tile_height() * m_console.get_height() * Constants::DEFAULT_PIXEL_SIZE;
			m_context = tcod::Context(params);
			m_mouse = {0, 0};
			m_quit = false;
			
			SDL_SetWindowHitTest(m_context.get_sdl_window(), Game::HitTestCallback, this);

			m_world = new World::World(World::CreateActor(World::ActorDefintion::PLAYER));
			set_event_handler(new GameplayEventHandler(this, m_world));

			srand(time(NULL) % 1000);
			regenerate_map();

			populate_window();

#ifdef RYSIC_DEBUG
			set_title("RYSIC " RYSIC_VERSION_STR);
#else
			set_title("RYSIC");
#endif
			set_fullscreen(false);
			SDL_ShowWindow(m_context.get_sdl_window());
		}
		catch(const std::exception& e)
		{
			fmt::print(stderr, "{}\n", e.what());
		}
	}

	Game::~Game()
	{
		m_window->remove_all();
		m_window->remove_all_widgets();
		m_console.release();
		m_context.close();

		s_instance = nullptr;
	}

	int Game::run()
	{
		fmt::print("starting...\n");
		while(!m_quit)
		{
			handle_events();
			m_canvas->clear();

			m_world->render(m_canvas->canvas, m_canvas->rect.w, m_canvas->rect.h);

			m_hp_bar->set(m_world->player()->stats.hp_ratio());
			m_window->render(m_console);
			m_context.present(m_console); // update console to screen
			TCOD_console_clear(m_console.get()); // Clear console
		}
		return m_exitCode;
	}

	void Game::quit(int code)
	{
		fmt::print("quitting{}...\n", code ? fmt::format(" with code {}", code) : "");
		m_exitCode = code;
		m_quit = true;
	}

	void Game::set_fullscreen(bool fullscreen)
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

	void Game::handle_events()
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			m_context.convert_event_coordinates(event); // convert pixel (screen space) to tile space
			Pos temp;
			if(m_window->handle_event(event, temp))
				continue;
			
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
			{ regenerate_map(); continue; }
			
			if(m_event_handler)
				m_event_handler->handle(event);
		}
	}

	void Game::set_title(const std::string &title)
	{
		SDL_SetWindowTitle(m_context.get_sdl_window(), title.c_str());
		m_window->title = title;
	}

	void Game::regenerate_map()
	{
		World::Map* new_map = World::MapGenerator::GenerateDungeon(Constants::DEFAULT_WIDTH, Constants::DEFAULT_HEIGHT - 1, 45, 6, 15, 2, m_world);
		m_world->set_map(new_map);
		m_world->set_player(World::CreateActor(World::ActorDefintion::PLAYER));
		m_event_handler = new GameplayEventHandler(this, m_world);
		m_world->log()->clear();
	}

	void Game::populate_window()
	{
		m_window = CreateRef<Interface::Window>(Rect{0, 0, m_width, m_height}, C_GRAY4, C_GRAY3, C_GRAY3, C_GRAY4);
		m_window->decoration = Interface::Frame::FrameDecoration::HEADER;

		auto btn_fullscreen = CreateRef<Interface::Button>(Rect{m_width - 9, 0, 3, 1}, "\u25b2", C_BLACK, C_GRAY2, C_GRAY3, C_GRAY0, [&](int, int, Interface::Button* const btn) {
			set_fullscreen(!m_fullscreen);
			if(m_fullscreen)
				btn->label = "\u25bc";
			else
				btn->label = "\u25b2";
		});
		m_window->add_widget(btn_fullscreen);

		auto close_button = CreateRef<Interface::Button>(Rect{m_width - 5, 0, 3, 1}, "X", C_WHITE, C_RED, [&](int, int, Interface::Button* const) { quit(); });
		m_window->add_widget(close_button);
		m_hp_bar = CreateRef<Interface::ProgressBar>(Pos{0, 2}, 15, "HP", C_WHITE, C_RED, TCOD_ColorRGB{40, 20, 20}, Interface::ProgressBar::Direction::VERTICAL);
		m_hp_bar->set(m_world->player()->stats.hp_ratio());
		m_window->add_widget(m_hp_bar);

		m_canvas = CreateRef<Interface::Canvas>(Rect{0, 0, m_window->rect.w, ((m_window->rect.h * 3) / 4)}, C_WHITE, C_BLACK);
		m_window->add(m_canvas);
		
		auto message_log = CreateRef<Interface::LogContainer>(Rect{0, (m_window->rect.h * 3) / 4, m_window->rect.w, (m_window->rect.h * 1) / 4}, m_world->log());
		message_log->fg = C_GRAY3;
		message_log->bg = C_GRAY0;
		m_window->add(message_log);

		m_world->message(
			"Use [wasd], [hjkl] or [numpad] to move.\nPress [r] to reveal map. Press [f] to forget.\nPress [space] to generate a new map. Press [esc] to exit.",
				Log::MessageType::SYSTEM);
	}

	SDL_HitTestResult Game::HitTestCallback(SDL_Window*, const SDL_Point *area, void *data)
	{
		Game *game = (Game*) data;
		auto mouse = game->m_context.pixel_to_tile_coordinates(std::array<int, 2>{area->x, area->y});
		return game->m_window->get_hittest_result({mouse[0], mouse[1]});
	}
}