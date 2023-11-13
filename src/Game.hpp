#include <SDL2/SDL.h>
#include <libtcod.hpp>

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
	uint32_t m_width, m_height;
	GameState m_gameState;

	public:
		Game(TCOD_ContextParams params, int width, int height);
		~Game();
		int run();
	};
}