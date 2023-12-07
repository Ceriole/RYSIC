#include "Map.hpp"

#include <queue>

#include "Actions.hpp"
#include "screen/ScreenUtil.hpp"
#include "Fov.hpp"
#include "Constants.hpp"

namespace RYSIC::World
{
	
	Map::Map( unsigned int width, unsigned int height, const Tile& fill_tile)
		: m_width(width), m_height(height)
	{
		const int tile_cnt = m_width * m_height;

		m_tiles = new Tile[tile_cnt];
		std::fill_n(m_tiles, tile_cnt, fill_tile);
		m_tile_attributes = new TileAttributes[tile_cnt];
		amnesia();
	}

	Map::~Map()
	{
		for(Entity* ent : m_entities)
			delete ent;
		m_entities.clear();
		delete m_tile_attributes;
		delete m_tiles;
	}

	Tile* Map::at(const Pos &xy) const
	{
		if(in_bounds(xy))
			return &m_tiles[xy.x + (xy.y * m_width)];
		return nullptr;
	}

	TileAttributes* Map::attrib_at(const Pos &xy) const
	{
		if(in_bounds(xy))
			return &m_tile_attributes[xy.x + (xy.y * m_width)];
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

	bool Map::in_bounds(const Pos& xy) const
	{
		return xy.x >= 0 && xy.y >= 0 && xy.x < (int) m_width && xy.y < (int) m_height;
	}

	std::set<Entity*> Map::entities_at(const Pos& xy) const
	{
		std::set<Entity*> found;
		for(auto ent : m_entities)
			if(ent->pos == xy)
				found.insert(ent);
		return found;
	}

	Entity* Map::blocking_entity_at(const Pos& xy) const
	{
		for(auto ent : entities_at(xy))
			if(ent->blocks_movement)
				return ent;
		return nullptr;
	}

	void Map::render(TCOD_Console &console) const
	{
		for(int x = 0; x < (int) m_width; x++)
			for(int y = 0; y < (int) m_height; y++)
			{
				Tile* tile = at({x, y});
				TileAttributes* tile_attrib = attrib_at({x, y});
				Glyph gfx;
				if(tile_attrib->visible)
					gfx = tile->gfx;
				else if(tile_attrib->explored)
					gfx = tile->gfx.dark(Constants::UNEXPLORED_DARK_FACTOR);
				else
					gfx = GLYPH_UNSEEN;
				Util::Screen::set_char(console, x, y, gfx);
			}
		for(auto ent : m_entities)
			if(attrib_at(ent->pos)->visible)
				ent->render(console);
	}

	void Map::update_fov(Entity* viewer)
	{
		if(viewer == nullptr)
			return;

		blind(); // clear visible tiles

		auto _blocks = [&](const Pos& xy)->bool { return blocks_sight(xy); };
		auto _reveal = [&](const Pos& xy)->void { reveal(xy); };
		FOV::compute(viewer->pos, _blocks, _reveal, viewer->get_vision_radius());
	}

	void Map::nightmare_eyes(const Pos& origin)
	{
		if(origin.positive())
			flood_reveal(origin, true);
		else
		{
			const int tile_cnt = m_width * m_height;
			std::fill_n(m_tile_attributes, tile_cnt, TileAttributes{true, true});
		}
	}

	void Map::amnesia()
	{
		const int tile_cnt = m_width * m_height;
		std::fill_n(m_tile_attributes, tile_cnt, TileAttributes{false, false});
	}

	void Map::blind()
	{
		const int tile_cnt = m_width * m_height;
		for(int i = 0; i < tile_cnt; i++)
			m_tile_attributes[i].visible = false;
	}

	void Map::flood_reveal(const Pos &origin, bool all_visible)
	{
		bool* vis = new bool[m_width * m_height];
		std::fill(vis, &vis[m_width * m_height], false);

		std::queue<Pos> q;
		q.push(origin);
		vis[origin.x + (origin.y * m_width)] = true;

		while(!q.empty())
		{
			Pos pos = q.front();
			if(all_visible)
				reveal(pos);
			else
				remember(pos);
			q.pop();
			if(blocks_sight(pos))
				continue;
			static const Pos AROUND[8] = {Pos{-1, -1}, Pos{0, -1}, Pos{1, -1}, Pos{-1, 0}, Pos{1, 0}, Pos{-1, 1}, Pos{0, 1}, Pos{1, 1}};
			for(int i = 0; i < 8; i++)
			{
				Pos next = pos + AROUND[i];
				if(in_bounds(next) && !vis[next.x + (next.y * m_width)])
				{
					q.push(next);
					vis[next.x + (next.y * m_width)] = true;
				}
			}
		}
	}

	bool Map::reveal(const Pos &xy)
	{
		TileAttributes* ta = attrib_at(xy);
		if(ta != nullptr)
		{
			bool old = ta->visible;
			ta->explored = ta->visible = true;
			return old != true;
		}
		return false;
	}

	bool Map::hide(const Pos & xy)
	{
		TileAttributes* ta = attrib_at(xy);
		if(ta != nullptr)
		{
			bool old = ta->visible;
			ta->visible = false;
			return old != false;
		}
		return false;
	}

	bool Map::remember(const Pos &xy)
	{
		TileAttributes* ta = attrib_at(xy);
		if(ta != nullptr)
		{
			bool old = ta->explored;
			ta->explored = true;
			return old != true;
		}
		return false;
	}

	bool Map::forget(const Pos & xy)
	{
		TileAttributes* ta = attrib_at(xy);
		if(ta != nullptr)
			ta->explored = false;
		return false;
	}

	bool Map::can_see(const Pos &origin, const Pos &target, double max_distance) const
	{
		auto _blocks = [&](const Pos& xy)->bool { return blocks_sight(xy); };
		return FOV::visible_between(origin, target, _blocks, max_distance);
	}

	bool Map::blocks_sight(const Pos& xy) const
	{
		Tile* tile = at(xy);
		if(tile)
			return !tile->transparent;
		return true;
	}

	void Map::progress(unsigned long tics)
	{
		// todo
	}
}