#pragma once

#include <libtcod.hpp>
#include "Map.hpp"

namespace RYSIC::World
{

	class World
	{
		Entity* m_player = nullptr;
		Map* m_current_map = nullptr;

	public:
		World(Entity* player);
		~World();

		Map* get_map() const { return m_current_map; }
		Entity* get_player() const { return m_player; }

		void set_map(Map* map, const Pos& player_xy);
		void render(TCOD_Console &console) const;
		void handle_action(Action* action);

	};

}