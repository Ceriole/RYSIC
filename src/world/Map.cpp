#include "Map.hpp"

#include "Actions.hpp"
#include "screen/ScreenUtil.hpp"

namespace RYSIC::World
{
	
	Map::Map( unsigned int width, unsigned int height, const Tile& fill_tile)
		: m_width(width), m_height(height)
	{
		m_tiles = new Tile[m_width * m_height];
		std::fill_n(m_tiles, m_width * m_height, fill_tile);
	}

	Map::~Map()
	{
		for(Entity* ent : m_entities)
			delete ent;
		m_entities.clear();
		delete m_tiles;
	}

	Tile* Map::at(const Pos &xy) const
	{
		if(in_bounds(xy))
			return &m_tiles[xy.x + (xy.y * m_width)];
		return nullptr;
	}
	
	Map *Map::add(Entity* entity)
	{
		m_entities.insert(entity);
		return this;
	}

	Map *Map::remove(Entity *entity)
	{
		m_entities.erase(entity);
		return this;
	}

	bool Map::in_bounds(const Pos &xy) const
	{
		return xy.x >= 0 && xy.y >= 0 && xy.x < (int) m_width && xy.y < (int) m_height;
	}

	void Map::render(TCOD_Console &console) const
	{
		for(int x = 0; x < (int) m_width; x++)
			for(int y = 0; y < (int) m_height; y++)
				Util::Screen::set_char(console, x, y, at({x, y})->gfx);

		for(auto ent : m_entities)
			ent->render(console);
	}
}