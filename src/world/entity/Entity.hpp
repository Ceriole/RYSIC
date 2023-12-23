#pragma once

#include <libtcod.hpp>
#include <string>

#include "Types.hpp"

namespace RYSIC::World
{

	class World;
	class Map;

	class Entity
	{
	protected:
		Map* m_map;
	public:
		Pos pos;
		Glyph gfx;
		std::string name = "<unnamed>";
		bool blocks_movement;
		enum RenderOrder
		{
			CORPSE = 0, ITEM, ACTOR
		} render_order;

	public:
		Entity() = default;
		Entity(Map* map, const Pos &_pos, const Glyph &_gfx, const std::string& _name, bool _blocks_movement,
			RenderOrder _render_order);

		void move(const Pos &d_pos);
		inline void move(int dx, int dy) { move({dx, dy}); }

		virtual void render(TCOD_Console &console) const;
		virtual void debug_render(TCOD_Console &) const {};

		Entity* spawn(Map* new_map, const Pos& xy) const;
		bool place(Map* new_map, const Pos& xy);

		Map* map() const { return m_map; }

		virtual void progress(unsigned long) {}

		virtual bool thinks() const { return false; }

		friend World;
	};

}