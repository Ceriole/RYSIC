#include "Game.hpp"

int main(int argc, char* argv[])
{
	auto params = TCOD_ContextParams{};
	params.argc = argc;
	params.argv = argv;
	auto game = RYSIC::Game(params, 80, 35);
	return game.run();
}