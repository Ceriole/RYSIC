#include "Map.hpp"

#include <queue>

#include "World.hpp"
#include "Actions.hpp"
#include "screen/ScreenUtil.hpp"
#include "Fov.hpp"
#include "Constants.hpp"
#include "entity/Actor.hpp"

namespace RYSIC::World
{
	
	Map::Map(World* world, unsigned int width, unsigned int height, const Tile& fill_tile)
		: m_world(world), m_width(width), m_height(height)
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
		if(!has_entity(entity))
			m_entities.insert(entity);
		return this;
	}

	Map *Map::remove(Entity *entity)
	{
		if(has_entity(entity))
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

	Actor *Map::actor_at(const Pos &xy) const
	{
		for(auto ent : entities_at(xy))
			if(dynamic_cast<Actor*>(ent) && ent->thinks())
				return (Actor*) ent;
		return nullptr;
	}

	struct render_order_sorter
	{
		inline bool operator() (const Entity* a, const Entity* b)
		{
			return a->render_order < b->render_order;
		}
	};

	void Map::render(TCOD_Console &console, const Pos& focus, unsigned int win_w, unsigned int win_h) const
	{
		tcod::Console map_console = Util::Screen::CreateComponentCanvas(m_width, m_height, std::nullopt, std::nullopt);
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
				Util::Screen::set_char(map_console, x, y, gfx);
			}
		std::vector<Entity*> render_ordered_entities(m_entities.begin(), m_entities.end());
		std::sort(render_ordered_entities.begin(), render_ordered_entities.end(), render_order_sorter());
		for(auto ent : render_ordered_entities)
			if(attrib_at(ent->pos)->visible)
				ent->render(map_console);
		
		Pos map_offset_pos = focus - Pos{(int) win_w / 2, (int) win_h / 2};
		tcod::blit(console, map_console, {0, 0}, {map_offset_pos.x, map_offset_pos.y, (int) win_w, (int) win_h});
	}

	void Map::update_fov(Entity* viewer)
	{
		if(viewer == nullptr)
			return;
		if(!has_entity(viewer))
			return;

		blind(); // clear visible tiles

		Actor* actor = dynamic_cast<Actor*>(viewer);
		if(actor)
		{
			auto _blocks = [&](const Pos& xy)->bool { return blocks_sight(xy); };
			auto _reveal = [&](const Pos& xy)->void { reveal(xy); };
			FOV::compute(viewer->pos, _blocks, _reveal, actor->stats.vision_radius());
		}
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

	float Map::get_movement_cost(const Pos &from, const Pos &to) const
	{
		const Tile* origin_tile = at(from);
		const Tile* dest_tile = at(to);
		if(origin_tile && dest_tile)
		{
			if(!origin_tile->walkable || !dest_tile->walkable)
				return 0.0f;
			return (origin_tile->walk_cost + dest_tile->walk_cost);
		}
		return 0.0f;
	}

	void Map::progress(unsigned long tics)
	{
		for(auto ent : m_entities)
			ent->progress(tics);
	}
}