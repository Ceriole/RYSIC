#include "Game.hpp"

#include <iostream>

#include "Constants.hpp"
#include "screen/ScreenUtil.hpp"
#include "world/Map.hpp"
#include "world/MapGenerator.hpp"

namespace RYSIC
{

	Game::Game(TCOD_ContextParams params, int width, int height)
		: m_width(width), m_height(height)
	{
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

			m_window = CreateRef<Interface::Window>(Rect{0, 0, m_width, m_height}, C_GRAY4, C_GRAY3, C_GRAY3, C_GRAY4);
			auto btn_fullscreen = CreateRef<Interface::Button>(Rect{m_width - 9, 0, 3, 1}, "\u25b2", C_BLACK, C_GRAY2, C_GRAY3, C_GRAY0, [&](int, int, Interface::Button* const btn) {
				set_fullscreen(!m_fullscreen);
				if(m_fullscreen)
					btn->label = "\u25bc";
				else
					btn->label = "\u25b2";
			});
			auto close_button = CreateRef<Interface::Button>(Rect{m_width - 5, 0, 3, 1}, "X", C_WHITE, C_RED, [&](int, int, Interface::Button* const) { quit(); });
			m_canvas = CreateRef<Interface::Canvas>(Rect{0, 0, m_window->rect.w, m_window->rect.h - 1}, C_WHITE, C_BLACK);
			m_window->decoration = Interface::Frame::FrameDecoration::HEADER;
			m_window->add(m_canvas);
			m_window->add_widget(close_button);
			m_window->add_widget(btn_fullscreen);
			SDL_SetWindowHitTest(m_context.get_sdl_window(), Game::HitTestCallback, this);

			m_world = new World::World(new World::Entity(
						{},
						{0x3100, {C_WHITE}, std::nullopt}
					));

			srand(time(NULL) % 1000);

			Pos player_pos;
			auto map = World::MapGenerator::GenerateDungeon(Constants::DEFAULT_WIDTH, Constants::DEFAULT_HEIGHT - 1, 45, 6, 15, &player_pos);
			/*map->add(
				new World::Entity(
					{25, 20},
					{0x3104, {C_YELLOW}, {C_BLACK}})
			);*/
			m_world->set_map(map, player_pos);

			set_title("RYSIC");
			set_fullscreen(false);
			SDL_ShowWindow(m_context.get_sdl_window());
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	Game::~Game()
	{
		m_window->remove_all();
		m_window->remove_all_widgets();
		m_console.release();
		m_context.close();
	}

	int Game::run()
	{
		while(!m_quit)
		{
			handle_events();
			m_canvas->clear();

			Util::Screen::fill(m_canvas->canvas, '.', C_GRAY0, C_BLACK);
			m_world->render(m_canvas->canvas);

			m_window->render(m_console);
			m_context.present(m_console); // update console to screen
			TCOD_console_clear(m_console.get()); // Clear console
		}
		return m_exitCode;
	}

	void Game::quit(int code)
	{
		printf("Quitting...\n");
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
			{
				auto new_map = World::MapGenerator::GenerateDungeon(Constants::DEFAULT_WIDTH, Constants::DEFAULT_HEIGHT - 1, 45, 6, 15, &temp);
				m_world->set_map(new_map, temp);
			}
			
			auto action = World::GetActionFromEvent(event);
			switch(action->type())
			{
			case World::Action::Type::EXIT:
				quit(); break;
			default:
				m_world->handle_action(action); break;
			}
		}
	}

	void Game::set_title(const std::string &title)
	{
		SDL_SetWindowTitle(m_context.get_sdl_window(), title.c_str());
		m_window->title = title;
	}

	SDL_HitTestResult Game::HitTestCallback(SDL_Window*, const SDL_Point *area, void *data)
	{
		Game *game = (Game*) data;
		auto mouse = game->m_context.pixel_to_tile_coordinates(std::array<int, 2>{area->x, area->y});
		return game->m_window->get_hittest_result({mouse[0], mouse[1]});
	}
}