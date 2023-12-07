#pragma once

#include <libtcod.hpp>
#include <string>

#include "Types.hpp"

namespace RYSIC::World
{

	struct Stats
	{
		double vision_radius = 8;
		unsigned long move_speed = 100;
		// todo add more stats
	};

	class Entity
	{
	public:
		Pos pos;
		Glyph gfx;
		Stats stats;
		std::string name;
		bool blocks_movement;

	public:
		Entity() = default;
		Entity(const Pos &_pos, const Glyph &_gfx, const std::string& _name, bool _blocks_movement)
			: pos(_pos), gfx(_gfx), name(_name), blocks_movement(_blocks_movement)
		{}

		void move(const Pos &d_pos);
		inline void move(int dx, int dy) { move({dx, dy}); }

		void render(TCOD_Console &console);

		double get_vision_radius() // todo calculate from other stats
		{ return stats.vision_radius; }

		Entity* spawn(class Map* map, const Pos& xy) const;
	};

}