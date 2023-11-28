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
		Character gfx;

		bool operator==(const Tile& rhs) const
		{ return memcmp(this, &rhs, sizeof(Tile)); }
	};

	constexpr Tile TILE_VOID = {false, false, {' ', std::nullopt, std::nullopt}};
	constexpr Tile TILE_WALL = {false, false, {0x3003, C_GRAY2, C_BLACK}};
	constexpr Tile TILE_FLOOR = {true, true, {0x3000, C_DARK_GREEN, {{0, 20, 0}}}};

	class Map
	{
	private:
		unsigned int m_width = 0, m_height = 0;
		Tile* m_tiles = nullptr;
		std::set<Entity*> m_entities;

	public:
		Map(unsigned int width, unsigned int height, const Tile& fill_tile = TILE_VOID);
		~Map();

		Tile* at(const Pos& xy) const;
		Map* add(Entity* entity);
		Map* remove(Entity* entity);
		bool in_bounds(const Pos& xy) const;

		void render(TCOD_Console &console) const;

		unsigned int width() const { return m_width; }
		unsigned int height() const { return m_height; }

		Tile& operator[](int index)
		{
			if(index >= (int)(m_width * m_height))
				throw std::runtime_error("Index out of array bounds");
			return m_tiles[index];
		};
	};

}