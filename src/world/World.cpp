#include "World.hpp"

#include <iostream>

#include "Constants.hpp"
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

	const char * World::get_time_string() const
	{
		unsigned int seconds = m_tics / Constants::WORLD_TICS_SECOND;
		unsigned int minutes = seconds / 60;
		unsigned int hours = minutes / 60;
		unsigned int days = hours / 24;
		unsigned int years = days / 365;
		static char buf[Constants::WORLD_TIME_STRLEN];
		sprintf_s(buf, Constants::WORLD_TIME_FORMAT, years % 999, days % 365, hours % 24, minutes % 60, seconds % 60);
		return buf;
	}

	void World::set_map(Map *map, const Pos &player_xy)
	{
		if(m_current_map)
		{
			m_current_map->remove(m_player);
			delete m_current_map;
		}
		m_current_map = map;
		if(m_current_map)
		{
			m_player->pos = player_xy;
			m_current_map->add(m_player);
			m_current_map->update_fov(m_player);
		}
	}

	void World::render(TCOD_Console &console) const
	{
		if(!m_current_map)
			return;
		m_current_map->render(console);
	}

	void World::handle_action(Action *action)
	{
		if(action == nullptr || m_current_map == nullptr)
			return;
		action->perform(this, m_player);
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