#include <SDL2/SDL.h>
#include <libtcod.hpp>

#define IS_WITHIN(X, Y, A_X, A_Y, A_W, A_H) ((X) >= (A_X) && (X) < ((A_X) + (A_W)) && (Y) >= (A_Y) && (Y) < ((A_Y) + (A_H)))
#define IS_AT(X, Y, P_X, P_Y) ((X) == (P_X) && (Y) == (P_Y))

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
	GameState m_gameState;
	bool m_quit, m_fullscreen;

	public:
		Game(TCOD_ContextParams params, int width, int height);
		~Game();
		int run();
	private:
		void drawWindow();
		void handleEvents();
		static SDL_HitTestResult hitTestCallback(SDL_Window *win, const SDL_Point *area, void *data);
	};
}