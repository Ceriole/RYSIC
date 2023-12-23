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
		else
			perform();
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
		bool has_seen_player = sees_player;
		Entity* target = world()->player();
		sees_player = map()->can_see(m_actor->pos, target->pos, m_actor->stats.vision_radius());
		if(sees_player)
		{
			if(!has_seen_player)
			{
				if(is_action<WaitAction>())
					start_action(nullptr);
				world()->announce(fmt::format("The {} points at you with malicious intent!", m_actor->name), m_actor->pos, Log::BAD);
			}
			last_seen_player_pos = target->pos;
			int d = m_actor->pos.chebyshev(target->pos);
			if(d <= 1)
			{
				start_action(new MeleeAction(world(), m_actor, target->pos - m_actor->pos));
				return;
			}
			m_path = get_path_to(target->pos);
			fmt::print("enemy path: {}\n", m_path);
		}

		if(!m_path.empty())
		{
			auto &dest = m_path.back();
			start_action(new MovementAction(world(), m_actor, dest - m_actor->pos));
			m_path.pop_back();
			return;
		}
		else if(rand() % 10 < 2)
			start_action(new MovementAction(world(), m_actor, {(rand() % 3) - 1, ((rand() % 3) - 1)}));
		else
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