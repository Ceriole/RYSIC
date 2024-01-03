#include "Actions.hpp"

#include "Constants.hpp"
#include "Game.hpp"
#include "world/World.hpp"
#include "world/Map.hpp"
#include "world/entity/Actor.hpp"

namespace RYSIC
{

	void ExitAction::perform()
	{
		m_game->quit();
	}

	void FullscreenToggleAction::perform()
	{
		m_game->set_fullscreen(!m_game->fullscreen());
	}
	
	void DebugToggleAction::perform()
	{
		m_game->set_debug(!m_game->debug());
		if(m_game->world())
			m_game->world()->message(fmt::format("Debug {}.", m_game->debug() ? "enabled" : "disabled"), Log::SYSTEM);
	}

	void WaitAction::perform()
	{
		if(m_entity == m_world->player())
		{
			m_world->message(Constants::MSG_WAIT);
			m_world->progress(tics);
		}
	}

	DirectionalAction::Direction DirectionalAction::dir() const
	{
		if(m_target.x == 0 && m_target.y == 0)
			return Direction::NONE;
		if(m_target.y < 0)
		{
			if(m_target.x < 0)
				return Direction::NORTHWEST;
			else if(m_target.x > 0)
				return Direction::NORTHEAST;
			else
				return Direction::NORTH;
		}
		else
		{
			if(m_target.x < 0)
				return Direction::SOUTHWEST;
			else if(m_target.x > 0)
				return Direction::SOUTHEAST;
			else
				return Direction::SOUTH;
		}
	}

	const Pos DirectionalAction::destination() const
	{
		return m_entity->pos + m_target;
	}

	World::Entity *DirectionalAction::get_blocking_entity() const
	{
		if(m_world->map())
			return m_world->map()->blocking_entity_at(destination());
		return nullptr;
	}

	World::Actor *DirectionalAction::get_actor() const
	{
		if(m_world->map())
			return m_world->map()->actor_at(destination());
		return nullptr;
	}

	void MovementAction::perform()
	{
		if(m_world->map())
		{
			Pos dest = m_entity->pos + m_target;

			auto target_tile = m_world->map()->at(dest);
			auto target_blocking_entity = m_world->map()->blocking_entity_at(dest);
			if(target_tile)
			{
				if(!target_tile->walkable)
					return;
				if(target_blocking_entity)
					return;
				m_entity->move(m_target);
				if(m_world->player() == m_entity)
				{
					if(World::Actor* actor = static_cast<World::Actor*>(m_entity))
						m_world->progress(actor->stats.move_speed());
					m_world->map()->update_fov(m_entity);
				}
			}
		}
		else
			m_entity->move(m_target);
	}

	void MeleeAction::perform()
	{
		if(m_world->map())
		{
			auto target_actor = get_actor();
			// todo attacking
			if(!target_actor)
				return;
			if(!target_actor->thinks())
				return;
			if(target_actor != m_entity)
			{
				auto actor = dynamic_cast<World::Actor*>(m_entity);
				if(!actor)
					return;
				actor->melee_attack(target_actor);
			}
		}
	}

	void VisibilityAction::perform()
	{
		if(m_world->map())
		{
			switch (viz_type)
			{
			case REVEAL: m_world->map()->flood_reveal(m_entity->pos); break;
			case HIDE:
				m_world->map()->amnesia();
				if(m_world->player() == m_entity)
					m_world->map()->update_fov(m_entity);
				break;
			case OMNIPOTENT: m_world->map()->nightmare_eyes(m_entity->pos); break;
			}
		}
	}

	void BumpAction::perform()
	{
		if(m_target.zero())
			return;
		if(m_world->map())
		{
			if(get_actor())
				MeleeAction(m_world, m_entity, m_target).perform();
			else
				MovementAction(m_world, m_entity, m_target).perform();
		}
	}

	void StartTargetAction::perform()
	{
		m_game->set_event_handler(new TargetEventHandler(m_game, m_world));
		m_world->targeting_start(m_origin, m_max_distance);
	}

	void MoveTargetAction::perform()
	{
		m_world->targeting_move(m_target);
	}

	void EndTargetAction::perform()
	{
		m_world->targeting_end();
		m_game->set_event_handler(new GameplayEventHandler(m_game, m_world));
	}

}
