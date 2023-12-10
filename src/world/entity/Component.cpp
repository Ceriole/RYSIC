#include "Component.hpp"

#include <math.h>
#include <libtcod.h>

#include "Constants.hpp"

#include "world/Map.hpp"
#include "Actor.hpp"

namespace RYSIC::World
{

	unsigned long RYSIC::World::StatComponent::move_speed() const
	{
		return MAX(
			Constants::STAT_MIN_MOVE_SPEED,
			Constants::STAT_BASE_MOVE_SPEED - tenacity.modifier(50.0f)
		);
	}

	unsigned int StatComponent::vision_radius() const
	{
		return MAX(
			1,
			Constants::STAT_BASE_VISION_RADIUS + sagacity.modifier(0.25f)
		);
	}

	int StatComponent::physical_defense() const
	{
		return Constants::STAT_BASE_MELEE_DEF + tenacity.modifier(0.75f) + vivacity.modifier(0.5f);
	}

	int StatComponent::psychic_defense() const
	{
		return Constants::STAT_BASE_PSYCHIC_DEF + sagacity.modifier(1.0f) + vivacity.modifier(0.25f);
	}

	int StatComponent::physical_power() const
	{
		return MAX(
			0,
			Constants::STAT_BASE_MELEE + tenacity.modifier(0.5f) - sagacity.modifier(0.25f)
		);
	}

	int StatComponent::psychic_power() const
	{
		return MAX(
			0,
			Constants::STAT_BASE_PSYCHIC + sagacity.modifier(0.5f) - tenacity.modifier(0.25f)
		);
	}

	int StatComponent::seduction_power() const
	{
		return MAX(
			0,
			Constants::STAT_BASE_SEDUCTION + vivacity.modifier(0.5f) + sagacity.modifier(0.25f)
		);
	}

	void StatComponent::modify_hp(int amount)
	{
		health.set(health.get() + amount);
		if(health.get() == 0 && m_actor)
			m_actor->die();
	}

	void StatComponent::recalculate()
	{
		health.set_max(MAX(
			1,
			Constants::STAT_BASE_HEALTH + tenacity.modifier(1.0f) + vivacity.modifier(0.5f)
		), true);
	}
	
	Map *BaseComponent::map() const
	{
		return m_actor->map();
	}

	World *BaseComponent::world() const
	{
		return map()->world();
	}
}