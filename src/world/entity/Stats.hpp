#pragma once

#include <libtcod.hpp>
#include <math.h>

namespace RYSIC::World::Stat
{

	struct Base
	{
	protected:
		int value;

	public:
		Base() : value(0) {}
		Base(int _value) : value(_value) {}

		virtual int get() const { return value; }
		virtual void set(int new_value) { value = new_value; }
		virtual void modify(int amount) { set(value + amount); }
	};

	template<int base_value>
	struct Attribute : public Base
	{
		Attribute() : Base(base_value) {}
		Attribute(int _value) : Base(_value) {}

		virtual void set(int new_value) override { value = MAX(0, new_value); } 
		int modifier(float factor = 1.0f) const
		{ return (int) floorf((float)(value - base_value) * factor); }
	};

	struct Bounded : public Base
	{
		int max;

		Bounded() : Base(0), max(0) {}
		Bounded(int _max) : Base(_max), max(_max) {}

		virtual void set(int new_value) override { value = CLAMP(0, max, new_value); }
		void set_max(int new_max_value, bool lerp = false)
		{
			float ratio = 1.0f;
			if(max > 0)
				ratio = (float) value / (float) max;
			max = MAX(0, new_max_value);
			if(lerp)
				set((int) floorf(ratio * (float) max));
			else
				set(value);
		}
	};

}