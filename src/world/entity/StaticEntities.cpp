#include "StaticEntities.hpp"

#include "Actor.hpp"

namespace RYSIC::World
{

	Actor *CreateActor(ActorDefintion type)
	{
		switch(type)
		{
		case ActorDefintion::PLAYER:
			return (new Actor(
				nullptr, {},
				{0x3100, {{255,255,255}}},
				"Player"
				))->set_ai<AI::BaseAI>();
		case ActorDefintion::KOBOLD:
			return (new Actor(
				nullptr, {},
				{0x3103, {{60, 100, 20}}},
				"Kobold", 3, 1, 1
				))->set_ai<AI::HostileAI>();
		case ActorDefintion::IMP:
			return (new Actor(
				nullptr, {},
				{0x310B, {{230, 85, 50}}},
				"Imp", 7, 8, 6
				))->set_ai<AI::HostileAI>();
				break;
		}
		return nullptr;
	}

}