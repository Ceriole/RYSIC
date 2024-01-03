#pragma once

#include <libtcod.hpp>
#include <math.h>
#include <fmt/core.h>

#include "Constants.hpp"


namespace RYSIC::World::Stat
{

	class Base
	{
	protected:
		int value;

	public:
		Base() { set(0); }
		Base(int _value) { set(_value); }

		virtual int get() const { return value; }
		virtual void set(int new_value) { value = new_value; }
		virtual void modify(int amount) { set(value + amount); }
	};

	template<int base_value>
	class Attribute : public Base
	{
	public:
		int mod;

		Attribute() : Base(base_value), mod(0) {}
		Attribute(int _value) : Base(_value), mod(0) {}

		virtual int get() const override { return MAX(0, value + mod); }
		virtual void set(int new_value) override { value = MAX(0, new_value); } 
		int modifier(float factor = 1.0f) const
		{ return (int) floorf((float)(get() - base_value) * factor); }
	};

	class Bounded : public Base
	{
	public:
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

	class Level : public Base
	{
	public:
		using ChangeCallback = std::function<void(int level, int previous)>;
	protected:
		int experience;
		ChangeCallback level_callback, xp_callback;

	public:
		Level() : Base(1), experience(0), level_callback(nullptr), xp_callback(nullptr) {}
		Level(int _value) : Base(_value) {}

		virtual int get() const { return value; }
		virtual int get_xp() const { return experience; }
		virtual int get_to_next() const { return Constants::ExperienceForLevel(value + 1); } // todo experience curve
		virtual void set(int new_value)
		{
			int old_value = value;
			value = MAX(1, new_value);
			if(value != old_value && level_callback)
				level_callback(value, old_value);
		}
		virtual void set_xp(int new_value)
		{
			int old_value = experience;
			experience = new_value;
			if(experience)
			{
				if(xp_callback)
					xp_callback(experience, old_value);
				while(experience >= get_to_next())
				{
					experience -= get_to_next();
					set(value + 1);
				}
			}
		}
		virtual void modify(int amount) { set(value + amount); }
		virtual void modify_xp(int amount) { set_xp(experience + amount); }

		void set_level_listener(ChangeCallback callback) { level_callback = callback; }
		void set_xp_listener(ChangeCallback callback) { xp_callback = callback; }
	};

}