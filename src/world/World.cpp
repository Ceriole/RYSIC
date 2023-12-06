#include "World.hpp"

#include "Map.hpp"

namespace RYSIC::World
{

	World::World(Entity *player)
		: m_player(player)
	{
	}

	World::~World()
	{
		set_map(nullptr, {0, 0});
		delete m_player;
	}

	void World::set_map(Map *map, const Pos& player_xy)
	{
		if(m_current_map != nullptr)
		{
			m_current_map->remove(m_player);
			delete m_current_map;
		}
		m_current_map = map;
		if(m_current_map != nullptr)
		{
			m_player->pos = player_xy;
			m_current_map->add(m_player);
			m_current_map->update_fov(m_player);
		}
	}

	void World::render(TCOD_Console &console) const
	{
		if(m_current_map == nullptr)
			return;
		m_current_map->render(console);
	}

	void World::handle_action(Action *action)
	{
		if(action == nullptr || m_current_map == nullptr)
			return;
		action->perform(this, m_player);
	}
}