#pragma once

#include <vector>

#include "Types.hpp"

// a lot of this is based upon https://www.albertford.com/shadowcasting/
// https://github.com/370417/symmetric-shadowcasting/
// https://github.com/370417/symmetric-shadowcasting/blob/master/LICENSE.txt

namespace RYSIC::World::FOV
{

	struct Quadrant
	{
		enum Cardinal { NORTH = 0, SOUTH, EAST, WEST } dir;
		Pos origin;
	
		Pos transform(const Pos& offset) const;
	};

	struct Row
	{
		int depth;
		double start_slope, end_slope;

		const std::vector<Pos> tiles() const;
		Row next() const;
	};

	double slope(const Pos& tile);
	bool is_symmetric(const Row& row, const Pos& tile);

	void compute(const Pos& origin, std::function<bool(const Pos&)> blocks_sight,
		std::function<void(const Pos&)> set_visible, double max_distance = 0);

}