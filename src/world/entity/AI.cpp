#include "AI.hpp"

#include <libtcod.h>

#include <fmt/ranges.h>

#include "world/World.hpp"
#include "world/Map.hpp"
#include "Actor.hpp"
#include "screen/ScreenUtil.hpp"

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

	void BaseAI::debug_render(TCOD_Console &console) const
	{
		if(action.current)
		{
			if(auto move_action = dynamic_cast<MovementAction*>(action.current))
			{
				auto [x, y] = move_action->destination();
				Util::Screen::set_char(console, x, y, {'x', C_YELLOW, {{255, 150, 0}}});
			}
		}
	}

	bool BaseAI::start_action(Action *new_action)
	{
		if(action.current && new_action)
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
		if(action.current && !is_action<WaitAction>()) // if there is a non-wait action queued up, skip the AI routine
			return;
		bool has_seen_target = sees_target;	// store if the actor has previously seen the player
		Entity* target = world()->player();	// store the target (player only for now)
		// check if actor can see target
		sees_target = map()->can_see(m_actor->pos, target->pos, m_actor->stats.vision_radius());
		if(sees_target) // if the actor sees the target
		{
			if(!has_seen_target) // if actor just saw the target
			{
				start_action(nullptr); // clear the current action
				world()->announce( // send a message to the log, the enemy is declaring intent!
					fmt::format("The {} points at you with malicious intent!", m_actor->name),
					m_actor->pos, Log::BAD);
			}
			int d = m_actor->pos.chebyshev(target->pos);
			// get the max distance between x and y positions between the actor and the target
			// chebyshev is not an absolute distance, the partial difference between the actor and the target
			// x or y, whatever is greater.
			if(d <= 1) // if the actor is inside(!?) or next to the target, start attacking!
			{
				start_action(new MeleeAction(world(), m_actor, target->pos - m_actor->pos));
				m_path.clear(); // clear the path
				return; // exit the routine
			}
			m_path = get_path_to(target->pos); // store the path to the target
		}

		if(!m_path.empty()) // if there is a path to the target
		{
			// move to the next position in the path, then remove that part of the path
			auto &dest = m_path.back();
			start_action(new MovementAction(world(), m_actor, dest - m_actor->pos));
			m_path.pop_back();
		}
		else if(rand() % 10 < 2) // if there is no path, either wander (20%)...
			start_action(new MovementAction(world(), m_actor, {(rand() % 3) - 1, ((rand() % 3) - 1)}));
		else // ... or wait
			start_action(new WaitAction(world(), m_actor, 10));
	}

	void HostileAI::debug_render(TCOD_Console& console) const
	{
		BaseAI::debug_render(console);
		if(!m_path.empty())
			for(auto [x, y] : m_path)
				Util::Screen::set_char(console, x, y, {'x', C_LIGHT_RED, C_RED});
	}

}