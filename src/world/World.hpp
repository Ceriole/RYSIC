#pragma once

#include <libtcod.hpp>

#include "Log.hpp"


namespace RYSIC::World
{
	class World;
	class Map;
	class Actor;

	class World
	{
	private:
		Actor* m_player = nullptr;
		Map* m_current_map = nullptr;
		unsigned long m_tics = 0;
		Log::MessageFeed m_log;
	public:
		World(Actor *player)
			: m_player(player)
		{}
		~World();
		Map* map() const { return m_current_map; }
		Actor* player() const { return m_player; }
		long time() const { return m_tics; }
		const std::string short_time_string() const;
		const std::string time_string() const;
		void set_map(Map* map);
		void set_player(Actor* player);
		void render(TCOD_Console &console, unsigned int win_w = 0, unsigned int win_h = 0) const;
		
		void progress(unsigned long tics);
		Log::MessageFeed* log() { return &m_log; };
		void message(const std::string& msg, Log::MessageType type = Log::MessageType::NORMAL);
		void announce(const std::string& msg, const Pos& origin, Log::MessageType type = Log::MessageType::NORMAL);
	};
}