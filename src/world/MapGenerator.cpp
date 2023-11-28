#include "MapGenerator.hpp"

#include "Types.hpp"


#define SIGN(N) ((N) < 0 ? -1 : ((N) > 0 ? 1 : 0))

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
		int dx = b.x - a.x, dy = b.y - a.y;
		int sx = SIGN(dx), sy = SIGN(dy);
		dx = abs(dx), dy = abs(dy);
		int d = MAX(dx, dy);
		double r = d / 2;
		Pos xy = a;

		if(dx > dy)
		{
			for(int i = 0; i < d; i++)
			{
				set_tile(map, xy, tile);
				xy.x += sx; r += dy;
				if(r >= dx)
					xy.y += sy, r -= dx;
			}
		}
		else
		{
			for(int i = 0; i < d; i++)
			{
				set_tile(map, xy, tile);
				xy.y += sy; r += dx;
				if(r >= dy)
					xy.y += sx, r -= dy;
			}
		}
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
		Map* map = new Map(width, height, TILE_WALL);

		std::vector<Rect> rooms;
		for(int r = 0; r < (int) max_rooms; r++)
		{
			int rw = room_min_size + (rand() % (room_max_size - room_min_size));
			int rh = room_min_size + (rand() % (room_max_size - room_min_size));

			int rx = rand() % (width - rw - 1);
			int ry = rand() % (height - rh - 1);

			Rect room = {rx, ry, rw, rh};

			bool intersects = false;
			for(auto old_room : rooms)
				if(room.intersects(old_room))
				{ intersects = true; break; }
			if(intersects)
				continue;

			carve(map, room.inner(), TILE_FLOOR);

			if(r == 0)
				(*player_pos) = room.center();
			else if(r == max_rooms - 1)
				carve_tunnel(map, rooms.front().center(), room.center(), TILE_FLOOR);
			else
				carve_tunnel(map, rooms.back().center(), room.center(), TILE_FLOOR);
			
			rooms.push_back(room);
		}

		return map;
	}
}
