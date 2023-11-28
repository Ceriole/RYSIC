// RYSIC main file (entrypoint)

#include "Game.hpp"
#include "Constants.hpp"

int main(int argc, char* argv[])
{
	auto params = TCOD_ContextParams{};
	params.argc = argc;
	params.argv = argv;
	auto game = RYSIC::Game(params, RYSIC::Constants::DEFAULT_WIDTH, RYSIC::Constants::DEFAULT_HEIGHT);
	return game.run();
}