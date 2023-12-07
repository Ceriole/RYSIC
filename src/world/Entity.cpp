#include "Entity.hpp"

#include "screen/ScreenUtil.hpp"
#include "Map.hpp"

namespace RYSIC::World
{
	void Entity::move(const Pos &d_pos)
	{
		pos += d_pos;
	}

	void Entity::render(TCOD_Console &console)
	{
		Util::Screen::set_char(console, pos.x, pos.y, gfx);
	}

	Entity* Entity::spawn(Map *map, const Pos &xy) const
	{
		if(map)
		{
			if(map->blocking_entity_at(xy) && blocks_movement)
				return nullptr;
			if(!map->at(xy)->walkable)
				return nullptr;
			Entity* e = new Entity(*this);
			e->pos = xy;
			map->add(e);
			return e;
			
		}
		return nullptr;
	}
}