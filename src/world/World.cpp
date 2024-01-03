#include "World.hpp"

#include <iostream>

#include "Constants.hpp"
#include "Map.hpp"
#include "entity/Actor.hpp"
#include "entity/AI.hpp"
#include "screen/ScreenUtil.hpp"
#include "Game.hpp"
#include "screen/GameInterface.hpp"

namespace RYSIC::World
{

	World::~World()
	{
		set_map(nullptr);
		delete m_player;
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
			m_current_map->update_fov(m_player);
	}

	void World::set_player(Actor *player)
	{
		if(!player)
			return;
		if(m_current_map)
			m_current_map->remove(m_player);
		Pos player_pos = m_player ? m_player->pos : player->pos;
		m_player = player;
		if(m_current_map)
			m_player->place(m_current_map, player_pos);
	}

	void World::render(TCOD_Console &console, unsigned int win_w, unsigned int win_h) const
	{
		if(!m_current_map)
			return;
		Pos cam_center = m_player->pos;
		if(m_target.enabled)
			cam_center = m_target.pos;
		m_current_map->render(console, cam_center, win_w, win_h);
		if(m_target.enabled && (SDL_GetTicks64() % 300) < 150)
			Util::Screen::set_char(console, win_w / 2, win_h / 2, GLYPH_TARGET);
	}

	void World::targeting_start(const Pos &origin, int max_distance)
	{
		if(m_target.enabled)
			return;
		m_target.enabled = true;
		m_target.origin = m_target.pos = origin;
		m_target.max_distance = max_distance;
	}

	void World::targeting_move(const Pos &target)
	{
		if(!m_target.enabled)
			return;
		Pos new_pos = m_target.pos + target;
		if(m_current_map)
		{
			new_pos.x = CLAMP(0, (int) m_current_map->width(), new_pos.x);
			new_pos.y = CLAMP(0, (int) m_current_map->height(), new_pos.y);
		}
		Pos diff = new_pos - m_target.origin;
		if(m_target.max_distance)
		{
			if(abs(diff.x) <= m_target.max_distance && abs(diff.y) <= m_target.max_distance)
				m_target.pos = new_pos;
		}
		else
			m_target.pos = new_pos;
		
		if(m_current_map && m_current_map->in_bounds(m_target.pos))
		{
			if(m_current_map->is_visible(m_target.pos))
			{
				if(auto actor = dynamic_cast<Actor*>(m_current_map->blocking_entity_at(m_target.pos)); actor)
					Game::Instance()->character_panel()->set_character(actor);
				else
					Game::Instance()->character_panel()->set_description("Tile", m_current_map->at(m_target.pos)->name);
			}
			else
				Game::Instance()->character_panel()->set_unseen();
		}
		else
			Game::Instance()->character_panel()->clear();
	}

	void World::targeting_end()
	{
		if(!m_target.enabled)
			return;
		if(m_target.action)
		{
			m_target.action->set(m_target.pos);
			m_target.action->perform();
			delete m_target.action;
		}
		m_target.enabled = false;
		m_target.origin = m_target.pos = {0,0};
		m_target.max_distance = 0;
		m_target.action = nullptr;
		Game::Instance()->character_panel()->set_character(m_player);
	}

	void World::progress(unsigned long tics)
	{
		m_time.progress(tics);
		if(m_current_map)
			m_current_map->progress(tics);
	}

	void World::message(const std::string_view msg, Log::MessageType type)
	{
		m_log.message({std::string(msg), type, m_time.tics});
	}

	void World::announce(const std::string_view msg, const Pos &origin, Log::MessageType type)
	{
		if(!m_current_map)
			return;
		if(m_current_map->can_see(m_player->pos, origin))
			message(msg, type);
	}
}