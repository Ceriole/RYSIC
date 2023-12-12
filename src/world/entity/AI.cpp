#include "AI.hpp"

#include <libtcod.h>

#include "world/World.hpp"
#include "world/Map.hpp"
#include "Actor.hpp"


namespace RYSIC::World::AI
{
	void BaseAI::progress(unsigned long tics)
	{
		perform();

		if(action.current)
		{
			if(action.delay <= tics)
			{
				action.current->perform();
				delete action.current;
				action.current = nullptr;
				action.delay = 0;
			}
			else
				action.delay -= tics;
		}
	}

	std::vector<Pos> BaseAI::get_path_to(const Pos &dest) const
	{
		TCODPath* path = new TCODPath(map()->width(), map()->height(), &path_callback, (void*) this);

		if(!path->compute(m_actor->pos.x, m_actor->pos.y, dest.x, dest.y))
		{
			delete path;
			return std::vector<Pos>();
		}

		path->reverse();
		std::vector<Pos> points;
		while(!path->isEmpty())
		{
			Pos current;
			path->walk(&current.x, &current.y, true);
			if(current == m_actor->pos) // skip first pos
				continue;
			points.push_back(current);
		}
		return points;
	}

	bool BaseAI::start_action(Action *new_action, bool priority)
	{
		if(!priority && action.current)
			return false;
		action.current = new_action;
		if(!action.current)
			return true;
		if(WaitAction* wait_action = dynamic_cast<WaitAction*>(action.current); wait_action)
			action.delay = wait_action->tics;
		else 
		{
			unsigned long delay = m_actor->stats.move_speed();
			if(MovementAction* move_action = dynamic_cast<MovementAction*>(action.current); move_action)
			{
				const Pos dest = move_action->destination();
				delay = (unsigned long) round((double) delay * map()->get_movement_cost(m_actor->pos, dest));
			}
			action.delay = delay;
		}
		return true;
	}

	float BaseAI::PathCallback::getWalkCost(int ox, int oy, int dx, int dy, void *data) const
	{
		const BaseAI* ai = (BaseAI*) data;
		const Pos origin(ox, oy), dest(dx, dy);
		const Entity* blocking_ent = ai->map()->blocking_entity_at(dest);
		const float tile_cost = ai->map()->get_movement_cost(origin, dest);
		if(tile_cost == 0.0f)
			return tile_cost;
		return blocking_ent ? tile_cost + 10.0f : tile_cost;
	}

	void HostileAI::perform()
	{
		Entity* target = world()->player();
		int d = m_actor->pos.chebyshev(target->pos);
		if(map()->can_see(m_actor->pos, target->pos, m_actor->stats.vision_radius()))
		{
			if(d <= 1)
			{
				start_action(new MeleeAction(world(), m_actor, target->pos - m_actor->pos), !is_action<MeleeAction>());
				return;
			}
			m_path = get_path_to(target->pos);
		}
		if(!m_path.empty())
		{
			Pos dest = m_path.back();
			m_path.pop_back();
			start_action(new MovementAction(world(), m_actor, dest - m_actor->pos));
			return;
		}
		
		if(rand() % 10 < 2)
			start_action(new MovementAction(world(), m_actor, {(rand() % 3) - 1, ((rand() % 3) - 1)}));
		else
			start_action(new WaitAction(world(), m_actor, 10));
	}

}