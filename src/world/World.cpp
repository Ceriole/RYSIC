#include "World.hpp"

#include <iostream>

#include "Constants.hpp"
#include "Map.hpp"
#include "entity/Actor.hpp"
#include "entity/AI.hpp"

namespace RYSIC::World
{

	World::~World()
	{
		set_map(nullptr);
		delete m_player;
	}

	const std::string World::short_time_string() const
	{
		return Util::short_time_to_string(time());
	}

	const std::string World::time_string() const
	{
		return Util::time_to_string(time());
	}

	void World::set_map(Map *map)
	{
		if(m_current_map)
		{
			m_current_map->remove(m_player);
			delete m_current_map;
			m_player->m_map = nullptr;
		}
		m_current_map = map;
		if(m_current_map)
		{
			m_current_map->update_fov(m_player);
		}
	}

	void World::render(TCOD_Console &console, unsigned int win_w, unsigned int win_h) const
	{
		if(!m_current_map)
			return;
		m_current_map->render(console, m_player->pos, win_w, win_h);
	}

	void World::progress(unsigned long tics)
	{
		m_tics += tics;
		if(m_current_map)
			m_current_map->progress(tics);
	}

	void World::message(const std::string &msg, Log::MessageType type)
	{
		m_log.message({msg, type, m_tics});
	}

	void World::announce(const std::string &msg, const Pos &origin, Log::MessageType type)
	{
		if(!m_current_map)
			return;
		if(m_current_map->can_see(m_player->pos, origin))
			message(msg, type);
	}
}