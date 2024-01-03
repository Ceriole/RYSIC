#include "StaticEntities.hpp"

#include "Actor.hpp"
#include "world/World.hpp"
#include "Game.hpp"
#include "Log.hpp"

namespace RYSIC::World
{

	Actor *CreateActor(ActorDefintion type)
	{
		Actor* actor = nullptr;
		switch(type)
		{
		case ActorDefintion::PLAYER:
			actor = new Actor(
				nullptr, {},
				{0x3100, {{255,255,255}}},
				"Player"
				);
			actor->stats.set_xp_listener(
				[](int xp, int previous)
				{
					if(xp > previous) Game::Instance()->world()->message(fmt::format("You have gained {} experience.", xp - previous), Log::GREAT);
					else if(xp < previous) Game::Instance()->world()->message(fmt::format("You have lost {} experience.", previous - xp), Log::TERRIBLE);
				}
			);
			actor->stats.set_level_listener(
				[&](int level, int previous)
				{
					if(level > previous)
					{
						Game::Instance()->world()->message(fmt::format("You have reached level {}!", level), Log::GREAT);
						// int level_increase = level - previous;
					}
				}
			);
			actor->set_ai<AI::BaseAI>();
			break;
		case ActorDefintion::KOBOLD:
			actor = new Actor(
				nullptr, {},
				{0x3103, {{60, 100, 20}}},
				"Kobold", 10, 3, 1, 1
				);
			actor->set_ai<AI::HostileAI>();
			break;
		case ActorDefintion::IMP:
			actor = new Actor(
				nullptr, {},
				{0x310B, {{230, 85, 50}}},
				"Imp", 50, 5, 6, 5
				);
			actor->set_ai<AI::HostileAI>();
			break;
		}
		return actor;
	}

}