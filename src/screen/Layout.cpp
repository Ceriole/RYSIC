#include "Layout.hpp"

namespace RYSIC::Interface
{

	void BorderLayout::add(Ref<Component> comp, Location location)
	{
		if(comp)
			m_components.push_back({comp, location});
	}

	bool BorderLayout::remove(Ref<Component> comp)
	{
		int removed = 0;
		m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
			[&](auto comp_pair) {
				if(comp_pair.first == comp)
				{ 
					removed++; return true;
				}
				return false;
			}
		));
		return removed;
	}

	void BorderLayout::clear()
	{
		m_components.clear();
	}

	bool BorderLayout::layout() const
	{
		int laid_out = 0;
		Ref<Component> locations[Location::Location_NR_ITEMS];

		const auto [px, py, pw, ph] = m_parent->rect;

		for(auto [comp, loc] : m_components)
		{
			if(!locations[loc])
			{
				locations[loc] = comp;
				laid_out++;
			}
		}

		int left = 0, right = pw - 1, top = 0, bottom = ph - 1;

		if(m_vertical) // vertical layout
		{
			if(auto comp = locations[Location::EAST]; comp)
			{
				right -= comp->rect.w;
				comp->rect.x = right;
				comp->rect.y = 0;
				comp->rect.h = ph;
			}
			if(auto comp = locations[Location::WEST]; comp)
			{
				left += comp->rect.w;
				comp->rect.x = 0;
				comp->rect.y = 0;
				comp->rect.h = ph;
			}
			if(auto comp = locations[Location::NORTH]; comp)
			{
				left += comp->rect.w;
				top += comp->rect.h;
				comp->rect.x = left;
				comp->rect.y = 0;
			}
			if(auto comp = locations[Location::SOUTH]; comp)
			{
				bottom -= comp->rect.h;
				comp->rect.x = right;
				comp->rect.y = bottom;
				comp->rect.w = right - left;
			}
		}
		else // horizontal layout
		{
			if(auto comp = locations[Location::NORTH]; comp)
			{
				top += comp->rect.h;
				comp->rect.x = 0;
				comp->rect.y = 0;
				comp->rect.w = pw;
			}
			if(auto comp = locations[Location::SOUTH]; comp)
			{
				bottom -= comp->rect.h;
				comp->rect.x = 0;
				comp->rect.y = bottom;
				comp->rect.w = pw;
			}
			if(auto comp = locations[Location::EAST]; comp)
			{
				right -= comp->rect.w;
				comp->rect.x = right;
				comp->rect.y = top;
				comp->rect.h = bottom - top;
			}
			if(auto comp = locations[Location::WEST]; comp)
			{
				left += comp->rect.w;
				comp->rect.x = left;
				comp->rect.y = top;
				comp->rect.h = bottom - top;
			}
		}
		if(auto comp = locations[Location::CENTER]; comp)
		{
			comp->rect.x = right;
			comp->rect.y = top;
			comp->rect.w = right - left;
			comp->rect.h = bottom - top;
		}
		return laid_out == m_components.size();
	}
}