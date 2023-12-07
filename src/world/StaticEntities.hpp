#pragma once

#include "Entity.hpp"

namespace RYSIC::World::EntityDefintions
{

	const static Entity PLAYER	({}, {0x3100, {{255,255,255}}}, "Player", true);
	const static Entity KOBOLD	({}, {0x3103, {{60, 100, 20}}}, "Kobold", true);
	const static Entity IMP		({}, {0x310B, {{230, 85, 50}}}, "Imp", true);

}