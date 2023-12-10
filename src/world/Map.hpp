#pragma once

#include <set>

#include "Types.hpp"

namespace RYSIC::World
{
	class World;
	class Entity;
	class Actor;

	struct Tile
	{
		bool walkable, transparent;
		Glyph gfx;
		float walk_cost = 1.0f;

		bool operator==(const Tile& rhs) const
		{ return memcmp(this, &rhs, sizeof(Tile)); }
	};

	struct TileAttributes
	{
		bool visible, explored;
	};

	constexpr Glyph GLYPH_UNSEEN =
		{' ', C_WHITE, std::nullopt};
	constexpr Tile TILE_VOID =
		{false, false, {' ', std::nullopt, std::nullopt}};
	constexpr Tile TILE_WALL =
		{false, false, {0x3003, C_GRAY4, C_BLACK}};
	constexpr Tile TILE_FLOOR =
		{true, true, {0x3000, {{20, 120, 20}}, {{0, 20, 0}}}, 1.0f};

	class Map
	{
	private:
		World* m_world;
		unsigned int m_width = 0, m_height = 0;
		Tile* m_tiles = nullptr;
		TileAttributes* m_tile_attributes = nullptr;
		std::set<Entity*> m_entities;

	public:
		Map(World* world, unsigned int width, unsigned int height, const Tile& fill_tile = TILE_VOID);
		~Map();

		Tile* at(const Pos& xy) const;
		TileAttributes* attrib_at(const Pos& xy) const;
		Map* add(Entity* entity);
		Map* remove(Entity* entity);
		bool has_entity(Entity* entity) const { return m_entities.find(entity) != m_entities.end(); };
		bool in_bounds(const Pos& xy) const;
		std::set<Entity*> entities_at(const Pos& xy) const;
		Entity* Map::blocking_entity_at(const Pos& xy) const; 
		Actor* Map::actor_at(const Pos& xy) const; 
		const std::set<Entity*> entities() const { return m_entities; }

		World* world() const { return m_world; }

		void render(TCOD_Console &console, const Pos& focus = {0, 0}, unsigned int win_w = 0, unsigned int win_h = 0) const;

		unsigned int width() const { return m_width; }
		unsigned int height() const { return m_height; }

		void update_fov(Entity* viewer);
		void nightmare_eyes(const Pos& origin = {-1, -1}); // rip A.H.
		void blind();
		void amnesia();

		void flood_reveal(const Pos& origin, bool all_visible = false);
		bool reveal(const Pos& xy);
		bool hide(const Pos& xy);
		bool remember(const Pos& xy);
		bool forget(const Pos& xy);

		float get_movement_cost(const Pos &from, const Pos &to) const;

		bool can_see(const Pos& origin, const Pos& target, double max_distance = 0) const;

		bool blocks_sight(const Pos& xy) const;

		void progress(unsigned long tics);

		Tile& operator[](int index)
		{
			if(index >= (int)(m_width * m_height))
				throw std::runtime_error("Index out of map bounds");
			return m_tiles[index];
		};
	};

}