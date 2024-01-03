#pragma once

#include "Stats.hpp"
#include "Constants.hpp"

namespace RYSIC::World
{
	class World;
	class Map;
	class Actor;

	struct BaseComponent
	{
	protected:
		Actor* m_actor;
	public:
		BaseComponent() {};
		BaseComponent(Actor* actor)
			: m_actor(actor)
		{}

		Map* map() const;
		World* world() const;
	};

	struct StatComponent : public BaseComponent
	{
	protected:
		Stat::Level _level;
		Stat::Bounded health;
		Stat::Attribute<Constants::STAT_BASE_VALUE> tenacity;
		Stat::Attribute<Constants::STAT_BASE_VALUE> vivacity;
		Stat::Attribute<Constants::STAT_BASE_VALUE> sagacity;

	public:
		StatComponent(Actor* actor)
			: BaseComponent(actor)
		{ recalculate(); }
		StatComponent(Actor* actor, int ten, int viv, int sag)
			: BaseComponent(actor), tenacity{ten}, vivacity{viv}, sagacity{sag}
		{ recalculate(); }

		void set_ten(int value) { tenacity.set(value); recalculate(); }
		void set_ten_mod(int value) { tenacity.mod = value; recalculate(); }
		int ten() const { return tenacity.get(); }
		int ten_mod() { return tenacity.mod; }
		
		void set_viv(int value) { vivacity.set(value); recalculate(); }
		void set_viv_mod(int value) { tenacity.mod = value; recalculate(); }
		int viv() const { return vivacity.get(); }
		int viv_mod() { return tenacity.mod; }

		void set_sag(int value) { sagacity.set(value); recalculate(); }
		void set_sag_mod(int value) { sagacity.mod = value; recalculate(); }
		int sag() const { return sagacity.get(); }
		int sag_mod() { return sagacity.mod; }

		unsigned long move_speed() const;
		unsigned int vision_radius() const;
		int physical_defense() const;
		int psychic_defense() const;
		int physical_power() const;
		int psychic_power() const;
		int seduction_power() const;

		int hp() const { return health.get(); };
		int max_hp() const { return health.max; };
		float hp_ratio() const { return CLAMP(0.0f, 1.0f, (float) hp() / (float) max_hp()); }
		void modify_hp(int amount);


		int level() const { return _level.get(); }
		int xp() const { return _level.get_xp(); }
		int xp_to_next() const { return _level.get_to_next(); }
		int xp_left() const { return xp_to_next() - xp(); }
		float xp_ratio() const { return CLAMP(0.0f, 1.0f, (float) xp() / (float) xp_to_next()); };
		void add_xp(int amount);
		void set_xp_listener(Stat::Level::ChangeCallback _callback) { _level.set_xp_listener(_callback); }
		void set_level_listener(Stat::Level::ChangeCallback _callback) { _level.set_level_listener(_callback); }

	private:
		void recalculate();
	};

}