#pragma once

#include <libtcod.hpp>

#include "Log.hpp"
#include "Constants.hpp"
#include "Time.hpp"
#include "Types.hpp"

namespace RYSIC
{
	class DirectionalAction;

	namespace World
	{
		class World;
		class Map;
		class Actor;

		constexpr Glyph GLYPH_TARGET =
			{0x4000, C_CYAN, std::nullopt};

		class World
		{
		private:
			Actor* m_player = nullptr;
			Map* m_current_map = nullptr;
			Time m_time;
			Log::MessageFeed m_log;
			struct Target
			{
				bool enabled = false;
				Pos origin = {0,0}, pos = {0,0};
				int max_distance = 0;
				DirectionalAction* action = nullptr;
			} m_target;
			
		public:
			World(Actor *player)
				: m_player(player)
			{}
			~World();
			Map* map() const { return m_current_map; }
			Actor* player() const { return m_player; }
			Time* time() { return &m_time; }
			void set_map(Map* map);
			void set_player(Actor* player);
			void render(TCOD_Console &console, unsigned int win_w = 0, unsigned int win_h = 0) const;

			bool targeting() const { return m_target.enabled; }
			Pos targeting_pos() const { return m_target.pos; }
			void targeting_start(const Pos &origin, int max_distance = 0);
			void targeting_move(const Pos &target);
			void targeting_end();
			
			void progress(unsigned long tics);
			Log::MessageFeed* log() { return &m_log; };
			void message(const std::string_view msg, Log::MessageType type = Log::MessageType::NORMAL);
			void announce(const std::string_view msg, const Pos& origin, Log::MessageType type = Log::MessageType::NORMAL);
		};
	}
	}