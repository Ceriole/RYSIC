#pragma once

#include <libtcod.hpp>

#include "Types.hpp"

namespace RYSIC::World
{

	class Entity
	{
	public:
		Pos pos;
		Character gfx;

	public:
		Entity() = default;
		Entity(const Pos &_pos, const Character &_gfx)
			: pos(_pos), gfx(_gfx)
		{}

		void move(const Pos &d_pos);
		inline void move(int dx, int dy) { move({dx, dy}); }

		void render(TCOD_Console &console);
	};

}