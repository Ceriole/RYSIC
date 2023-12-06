#pragma once

#include <libtcod.hpp>

#include "Types.hpp"

namespace RYSIC::World
{

	struct Stats
	{
		double vision_radius = 8;
		// todo add more stats
	};

	class Entity
	{
	public:
		Pos pos;
		Glyph gfx;
		Stats stats;

	public:
		Entity() = default;
		Entity(const Pos &_pos, const Glyph &_gfx)
			: pos(_pos), gfx(_gfx)
		{}

		void move(const Pos &d_pos);
		inline void move(int dx, int dy) { move({dx, dy}); }

		void render(TCOD_Console &console);

		double get_vision_radius() // todo calculate from other stats
		{ return stats.vision_radius; }
	};

}