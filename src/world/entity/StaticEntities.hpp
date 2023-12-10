#pragma once

namespace RYSIC::World
{
	class Actor;

	enum ActorDefintion
	{
		PLAYER, KOBOLD, IMP
	};

	Actor* CreateActor(ActorDefintion type);

}