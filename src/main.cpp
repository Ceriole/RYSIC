// RYSIC main file (entrypoint)

#include "Game.hpp"
#include "Constants.hpp"

int main(int argc, char* argv[])
{
	SDL_version sdl_ver;
	SDL_GetVersion(&sdl_ver);
	printf("sdl %u.%u.%u\n" TCOD_STRVERSIONNAME "\n", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
	auto params = TCOD_ContextParams{};
	params.argc = argc;
	params.argv = argv;
	auto game = RYSIC::Game(params, RYSIC::Constants::DEFAULT_WIDTH, RYSIC::Constants::DEFAULT_HEIGHT);
	return game.run();
}