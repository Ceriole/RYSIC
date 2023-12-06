#include "MapGenerator.hpp"

#include "Types.hpp"
#include "screen/LineUtil.hpp"
#include "Constants.hpp"

namespace RYSIC::World::MapGenerator
{

	void set_tile(Map* map, const Pos& pos, const Tile& tile)
	{
		Tile* tile_ptr = map->at(pos);
		if(tile_ptr != nullptr)
			(*tile_ptr) = tile;
	}

	void set_tile_rect(Map* map, const Rect& rect, const Tile& tile)
	{
		for(int x = 0; x < rect.w; x++)
			for(int y = 0; y < rect.h; y++)
				set_tile(map, {rect.x + x, rect.y + y}, tile);
	}
	
	void carve(Map* map, const Rect& room, const Tile& tile)
	{
		set_tile_rect(map, room.inner(), tile);
	}

	void carve_line(Map* map, const Pos& a, const Pos& b, const Tile& tile)
	{
		std::vector<Pos> line = Util::Line::Walk(a, b);
		for(auto p : line)
			set_tile(map, p, tile);
	}

	void carve_tunnel(Map* map, const Pos& a, const Pos& b, const Tile& tile)
	{
		Pos corner;
		if(rand() % 2)
			corner = {b.x, a.y};
		else
			corner = {a.x, b.y};

		carve_line(map, a, corner, tile);
		carve_line(map, corner, b, tile);
	}

	Map* GenerateDungeon(unsigned int width, unsigned int height, unsigned int max_rooms,
		unsigned int room_min_size, unsigned int room_max_size, Pos* player_pos)
	{
		Map* map = new Map(width, height, TILE_WALL);	// create map object

		std::vector<Rect> rooms;						// room array
		unsigned int r = 0, t = 0; 						// room count, tries counter
		while(r < (int) max_rooms)						// loop until room count == max_rooms
		{
			int rw = room_min_size + (rand() % (room_max_size - room_min_size));	// roll for room width
			int rh = room_min_size + (rand() % (room_max_size - room_min_size));	// roll for room height
			int rx = rand() % (width - rw - 1);			// roll for room x pos
			int ry = rand() % (height - rh - 1);		// roll for room y pos

			Rect room = {rx, ry, rw, rh};				// create room object (struct)

			bool collision = false;						// room collision flag
			for(auto old_room : rooms)
				if(room.intersects(old_room))
				{ collision = true; break; }
			
			t++;										// increment tries
			if(t == Constants::ROOM_GEN_MAX_ATTEMPTS)				// if tries == ROOM_GEN_MAX_ATTEMPTS
			{ t = 0; r++; continue; }					// increment rooms and continue (skip room)

			if(collision)								// if room collides with any other rooms,
				continue;								// skip carving the room and retry to reroll the room

			t = 0;										// reset tries
			carve(map, room.inner(), TILE_FLOOR);		// set map tiles where room is to TILE_FLOOR

			if(r == 0)									// if this is the first room, put the player there
				(*player_pos) = room.center();
			else if(r == max_rooms - 1)					// otherwise, if this is the last room,
														// make a tunnel to the first room
				carve_tunnel(map, rooms.front().center(), room.center(), TILE_FLOOR);
			else										// if the above two conditions are false,
														// make a tunnel from the previous room to the current one
				carve_tunnel(map, rooms.back().center(), room.center(), TILE_FLOOR);
			
			rooms.push_back(room);						// add room to room array (previously generated room list)
			r++;										// increment room count
		}

		return map;										// return the map object with the rooms carved.
	}
}
