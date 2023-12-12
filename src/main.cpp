// RYSIC main file (entrypoint)

#include <fmt/core.h>

#include "Game.hpp"
#include "Constants.hpp"
#include "Version.h"

#ifdef RYSIC_DEBUG
inline void print_libs()
{
	SDL_version sdl_ver;
	SDL_GetVersion(&sdl_ver);
	fmt::print("sdl {}.{}.{}\n", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
	fmt::print(TCOD_STRVERSIONNAME "\n");
	fmt::print("fmt {}.{}.{}\n", FMT_VERSION / 10000, (FMT_VERSION / 100) % 100, FMT_VERSION % 100);
}
#endif

int main(int argc, char* argv[])
{
#ifdef RYSIC_DEBUG
	fmt::print("RYSIC {}\n", RYSIC_VERSION);
	print_libs();
#endif
	auto params = TCOD_ContextParams{};
	params.argc = argc;
	params.argv = argv;
	auto game = RYSIC::Game(params, RYSIC::Constants::DEFAULT_WIDTH, RYSIC::Constants::DEFAULT_HEIGHT);
	return game.run();
}