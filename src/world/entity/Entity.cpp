#include "Entity.hpp"

#include "screen/ScreenUtil.hpp"
#include "world/Map.hpp"

namespace RYSIC::World
{
	Entity::Entity(Map *map, const Pos &_pos, const Glyph &_gfx, const std::string &_name, bool _blocks_movement,
		RenderOrder _render_order)
			: m_map(map), pos(_pos), gfx(_gfx), name(_name), blocks_movement(_blocks_movement),
			render_order(_render_order)
	{
		if(m_map)
			m_map->add(this);
	}

	void Entity::move(const Pos &d_pos)
	{
		pos += d_pos;
	}

	void Entity::render(TCOD_Console &console)
	{
		Util::Screen::set_char(console, pos.x, pos.y, gfx);
	}

	Entity* Entity::spawn(Map *new_map, const Pos &xy) const
	{
		if(new_map)
		{
			if(new_map->blocking_entity_at(xy) && blocks_movement)
				return nullptr;
			if(!new_map->at(xy)->walkable)
				return nullptr;
			Entity* e = new Entity(*this);
			e->pos = xy;
			e->m_map = new_map;
			new_map->add(e);
			return e;
		}
		return nullptr;
	}

	bool Entity::place(Map *new_map, const Pos &xy)
	{
		pos = xy;
		if(new_map)
		{
			if(new_map->blocking_entity_at(xy) && blocks_movement)
				return false;
			if(m_map)
				m_map->remove(this);
			m_map = new_map;
			new_map->add(this);
		}
		return true;
	}
}