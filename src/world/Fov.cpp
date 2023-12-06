#include "Fov.hpp"

namespace RYSIC::World::FOV
{

	Pos Quadrant::transform(const Pos &offset) const
	{
		switch (dir)
		{
		case NORTH:	return {origin.x + offset.x, origin.y -  offset.y};
		case SOUTH:	return {origin.x + offset.x, origin.y +  offset.y};
		case EAST:	return {origin.x + offset.y, origin.y +  offset.x};
		case WEST:	return {origin.x - offset.y, origin.y +  offset.x};
		}
		return origin;
	}

	inline int round_tile_up(double col)
	{
		return (int) floor(col + 0.5);
	}

	inline int round_tile_down(double col)
	{
		return (int) ceil(col - 0.5);
	}

	const std::vector<Pos> Row::tiles() const
	{
		int min_col = round_tile_up(depth * start_slope);
		int max_col = round_tile_down(depth * end_slope);
		std::vector<Pos> tiles;
		for(int col = min_col; col <= max_col; col++)
			tiles.push_back({col, depth});
		return tiles;
	}

	Row Row::next() const
	{
		return {depth + 1, start_slope, end_slope};
	}

	double slope(const Pos &tile)
	{
		return (2.0 * tile.x - 1.0) / (2.0 * tile.y);
	}


	bool is_symmetric(const Row& row, const Pos& tile)
	{
		return (tile.x >= row.depth * row.start_slope)
			&& (tile.x <= row.depth * row.end_slope);
	}

	void compute(const Pos &origin, std::function<bool(const Pos &)> blocks_sight, std::function<void(const Pos &)> set_visible, double max_distance)
	{
		set_visible(origin);
		
		for(int i = 0; i < 4; i++)
		{
			Quadrant quad = {(Quadrant::Cardinal) i, origin};
			auto is_opaque = [&](const Pos& tile)->bool { if(tile.negative()) return false; return blocks_sight(quad.transform(tile)); };
			auto is_transparent = [&](const Pos& tile)->bool { if(tile.negative()) return false; return !blocks_sight(quad.transform(tile)); };
			auto reveal = [&](const Pos& tile)->void { set_visible(quad.transform(tile)); };

			std::function<void(Row& row)> scan;
			scan = [&](Row row)
			{
				Pos prev_tile = {-1, -1};
				for(auto tile : row.tiles())
				{
					if(max_distance > 0 && tile.distance({0, 0}) >= max_distance)
						continue;

					if(is_opaque(tile) || is_symmetric(row, tile))	// if this tile is opaque or symmetric,
						reveal(tile);								// reveal it

					if(is_opaque(prev_tile) && is_transparent(tile))	// if this tile is transparent and the last is a not
						row.start_slope = slope(tile);					// set the start slope to point to the tile

					if(is_transparent(prev_tile) && is_opaque(tile))	// if the last tile is a floor and this is a wall
					{
						Row next_row = row.next();					// get the next row,
						next_row.end_slope = slope(tile);			// set the end slope of the next row to this tile,
						scan(next_row);								// and scan the next row (recursive)
					}
					prev_tile = tile;
				}
				if(is_transparent(prev_tile))							// if the last tile in the row is transparent
				{
					Row next_row = row.next();
					scan(next_row);								// scan the next row
				}
			};

			Row first_row = {1, -1, 1};
			scan(first_row);
		}
	}
}