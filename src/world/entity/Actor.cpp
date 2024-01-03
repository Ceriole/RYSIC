#include "Actor.hpp"

#include "world/World.hpp"
#include "world/Map.hpp"
#include "EventHandler.hpp"
#include "Game.hpp"

namespace RYSIC::World
{

	void Actor::debug_render(TCOD_Console &console) const
	{
		if(ai)
			ai->debug_render(console);
	}

	void Actor::melee_attack(Actor *target)
	{
		World* world = map()->world();
		int damage = stats.physical_power() - target->stats.physical_defense();
		std::string attack_desc = fmt::format("{} attacks {}, ", name, target->name);
		if(damage > 0)
			attack_desc += fmt::format("dealing {} damage!", damage);
		else
			attack_desc += "but deals no damage.";
		world->announce(attack_desc, pos);
		if(damage > 0)
			target->stats.modify_hp(-damage);
		if(!target->thinks()) // target dies
			stats.add_xp(target->defeat_xp);
		if(world->player() == this)
			world->progress(stats.move_speed());
	}

	bool Actor::is_player() const
	{
		 return this == m_map->world()->player();
	}

	void Actor::die()
	{
		if(is_player())
		{
			m_map->world()->announce(Constants::MSG_PLAYER_DEATH, pos, Log::TERRIBLE);
			Game::Instance()->set_event_handler(new GameOverEventHandler(Game::Instance()));
		}
		else
			m_map->world()->announce(fmt::format(Constants::MSG_DEATH, name), pos, Log::GOOD);
		gfx = gfx.dark();
		gfx.ch = '%';
		blocks_movement = false;
		delete ai;
		ai = nullptr;
		if(!is_player())
			name = fmt::format("{} corpse", name);
		render_order = RenderOrder::CORPSE;

	}

}