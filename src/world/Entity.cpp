#include "Entity.hpp"

#include "screen/ScreenUtil.hpp"

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
}