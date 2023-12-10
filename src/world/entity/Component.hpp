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

		void set_tenacity(int value) { tenacity.set(value); recalculate(); }
		int get_tenacity() const { return tenacity.get(); }

		void set_vivacity(int value) { vivacity.set(value); recalculate(); }
		int get_vivacity() const { return vivacity.get(); }

		void set_sagacity(int value) { sagacity.set(value); recalculate(); }
		int get_sagacity() const { return sagacity.get(); }

		unsigned long move_speed() const;
		unsigned int vision_radius() const;
		int physical_defense() const;
		int psychic_defense() const;
		int physical_power() const;
		int psychic_power() const;
		int seduction_power() const;

		int hp() const { return health.get(); };
		int max_hp() const { return health.max; };
		float hp_ratio() const { return (float) hp() / (float) max_hp(); }
		void modify_hp(int amount);
	private:
		void recalculate();
	};

}