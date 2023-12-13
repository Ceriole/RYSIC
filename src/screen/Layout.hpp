#pragma once

#include <vector>

#include "Interface.hpp"
#include "Pointers.hpp"
#include "Types.hpp"

namespace RYSIC::Interface
{

	class LayoutManager
	{
	protected:
		Ref<Container> m_parent;

	public:
		LayoutManager(Ref<Container> parent)
			: m_parent(parent)
		{}

		virtual void add(Ref<Component> comp) = 0;
		virtual bool remove(Ref<Component> comp) = 0;
		virtual void clear() = 0;

		virtual bool layout() const = 0;
	};

	class BorderLayout : public LayoutManager
	{
	public:
		enum Location
		{ CENTER, NORTH, SOUTH, EAST, WEST, Location_NR_ITEMS };
	private:
		bool m_vertical;
		std::vector<std::pair<Ref<Component>, Location>> m_components;

	public:
		BorderLayout(Ref<Container> parent, bool vertical = false)
			: LayoutManager(parent), m_vertical(vertical)
		{}

		virtual void add(Ref<Component> comp) override { add(comp, Location::CENTER); }
		virtual void add(Ref<Component> comp, Location location = Location::CENTER);
		virtual bool remove(Ref<Component> comp) override;
		virtual void clear() override;

		virtual bool layout() const override;
	private:
		using LayoutManager::add;
	};

}