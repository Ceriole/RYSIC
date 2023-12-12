#include "Actor.hpp"

#include "world/World.hpp"
#include "world/Map.hpp"
#include "EventHandler.hpp"
#include "Game.hpp"

namespace RYSIC::World
{
	void Actor::die()
	{
		std::string death_message;
		RYSIC::Log::MessageType message_type;
		if(this == m_map->world()->player())
		{
			death_message = Constants::MSG_PLAYER_DEATH, message_type = RYSIC::Log::TERRIBLE;
			Game::Instance()->set_event_handler(new GameOverEventHandler(Game::Instance()));
			map()->blind();
		}
		else
			death_message = tcod::stringf(Constants::MSG_DEATH, name.c_str()), message_type = RYSIC::Log::GOOD;
		gfx = gfx.dark();
		gfx.ch = '%';
		blocks_movement = false;
		delete ai;
		ai = nullptr;
		name = tcod::stringf("%s corpse", name.c_str());
		render_order = RenderOrder::CORPSE;

		m_map->world()->announce(death_message, pos, message_type);
	}

}