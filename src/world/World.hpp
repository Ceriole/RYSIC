#pragma once

#include <libtcod.hpp>

#include "Map.hpp"
#include "Log.hpp"

namespace RYSIC::World
{

	extern World* s_world_instance;

	class World
	{
	private:
		Entity* m_player = nullptr;
		Map* m_current_map = nullptr;
		unsigned long m_tics;
		Log::MessageFeed m_log;

	public:
		World(Entity* player);
		~World();

		Map* get_map() const { return m_current_map; }
		Entity* get_player() const { return m_player; }

		long get_time() const { return m_tics; }
		const char* get_time_string() const;

		void set_map(Map* map, const Pos& player_xy);
		void render(TCOD_Console &console, unsigned int win_w = 0, unsigned int win_h = 0) const;
		void handle_action(Action* action);

		void progress(unsigned long tics);

		Log::MessageFeed* log() { return &m_log; };

		void message(const std::string& msg, Log::MessageType type = Log::MessageType::NORMAL);
		void announce(const std::string& msg, const Pos& origin, Log::MessageType type = Log::MessageType::NORMAL);

	};

}