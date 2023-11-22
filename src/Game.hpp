#pragma once

#include <SDL2/SDL.h>
#include <libtcod.hpp>

#include "Colors.hpp"
#include "Interface.hpp"

namespace RYSIC
{
	enum GameState : uint8_t
	{
		GS_CSFT = 0x00,
		GS_TITL,
		GS_OPTN,
		GS_CHAR,
		GS_PLAY,
		GS_OVER
	};

	class Game
	{
	private:
	tcod::Context m_context;
	tcod::Console m_console;
	int m_width, m_height;
	struct s_mouse { int x, y; } m_mouse;
	GameState m_gameState = GS_CSFT;
	int m_gs_ctr = 0;
	long long m_gs_timer = 0;
	bool m_quit, m_fullscreen;
	int m_exitCode = 0;
	std::vector<SDL_Keycode> m_keys_down, m_keys_pressed;
	Interface::Window window;

	public:
		Game(TCOD_ContextParams params, int width, int height);
		~Game();
		int run();
		void quit(int code = 0);
		void setFullscreen(bool fullscreen);
		bool isKeyDown(SDL_Keycode key);
		bool isKeyPressed(SDL_Keycode key);
	private:
		void drawWindow();
		void handleEvents();
		void doState();
		static SDL_HitTestResult hitTestCallback(SDL_Window *win, const SDL_Point *area, void *data);

		void doStateCSFT();
		void doStateTITL();
	};
}