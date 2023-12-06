#pragma once

#include <set>

#include "Entity.hpp"
#include "Actions.hpp"
#include "Pointers.hpp"

namespace RYSIC::World
{
	
	struct Tile
	{
		bool walkable, transparent;
		Glyph gfx, dark_gfx;

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
		{false, false, {' ', std::nullopt, std::nullopt}, {' ', std::nullopt, std::nullopt}};
	constexpr Tile TILE_WALL =
		{false, false, {0x3003, C_GRAY4, C_BLACK}, {0x3003, C_GRAY1, C_BLACK}};
	constexpr Tile TILE_FLOOR =
		{true, true, {0x3000, {{20, 120, 20}}, {{0, 20, 0}}}, {0x3000, {{10, 60, 10}}, {{0, 5, 0}}}};

	class Map
	{
	private:
		unsigned int m_width = 0, m_height = 0;
		Tile* m_tiles = nullptr;
		TileAttributes* m_tile_attributes = nullptr;
		std::set<Entity*> m_entities;

	public:
		Map(unsigned int width, unsigned int height, const Tile& fill_tile = TILE_VOID);
		~Map();

		Tile* at(const Pos& xy) const;
		TileAttributes* attrib_at(const Pos& xy) const;
		Map* add(Entity* entity);
		Map* remove(Entity* entity);
		bool in_bounds(const Pos& xy) const;

		void render(TCOD_Console &console) const;

		unsigned int width() const { return m_width; }
		unsigned int height() const { return m_height; }

		void update_fov(Entity* viewer);
		void nightmare_eyes(); // rip A.H.
		void blind();
		void amnesia();

		void flood_reveal(const Pos& origin);
		bool reveal(const Pos& xy);
		bool hide(const Pos& xy);
		bool remember(const Pos& xy);
		bool forget(const Pos& xy);

		bool blocks_sight(const Pos& xy) const;

		Tile& operator[](int index)
		{
			if(index >= (int)(m_width * m_height))
				throw std::runtime_error("Index out of array bounds");
			return m_tiles[index];
		};
	};

}