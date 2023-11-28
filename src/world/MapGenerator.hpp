#pragma once

#include "Map.hpp"

namespace RYSIC::World::MapGenerator
{
	
	Map* GenerateDungeon(unsigned int width, unsigned int height, unsigned int max_rooms,
		unsigned int room_min_size, unsigned int room_max_size, Pos* player_pos);

}