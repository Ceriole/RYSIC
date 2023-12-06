#include "Actions.hpp"

#include "World.hpp"

namespace RYSIC::World
{

	Action* GetActionFromEvent(SDL_Event &event)
	{
		if(event.type == SDL_QUIT)
			return new ExitAction();
		if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return new ExitAction(); // quit
			case SDLK_UP:
			case SDLK_KP_8:
			case SDLK_j:
				return new MovementAction(0, -1); // up
			case SDLK_DOWN:
			case SDLK_KP_2:
			case SDLK_k:
				return new MovementAction(0, 1); // down
			case SDLK_LEFT:
			case SDLK_KP_4:
			case SDLK_h:
				return new MovementAction(-1, 0); // left
			case SDLK_RIGHT:
			case SDLK_KP_6:
			case SDLK_l:
				return new MovementAction(1, 0); // right
			case SDLK_KP_7:
				return new MovementAction(-1, -1); // up-left
			case SDLK_KP_9:
				return new MovementAction(1, -1); // up-right
			case SDLK_KP_1:
				return new MovementAction(-1, 1); // down-left
			case SDLK_KP_3:
				return new MovementAction(1, 1); // down-right
			case SDLK_PERIOD:
			case SDLK_KP_5:
				return new MovementAction(0, 0); // move in place (wait a turn)
			case SDLK_r:
				return new VisibilityAction(VisibilityAction::REVEAL); // reveal 
			case SDLK_f:
				return new VisibilityAction(VisibilityAction::HIDE); // hide 
			}
		}
		return new Action();
	}

	MovementAction::Direction MovementAction::dir() const
	{
		if(dx == 0 && dy == 0)
			return Direction::NONE;
		if(dy < 0)
		{
			if(dx < 0)
				return Direction::NORTHWEST;
			else if(dx > 0)
				return Direction::NORTHEAST;
			else
				return Direction::NORTH;
		}
		else
		{
			if(dx < 0)
				return Direction::SOUTHWEST;
			else if(dx > 0)
				return Direction::SOUTHEAST;
			else
				return Direction::SOUTH;
		}
	}

	void Action::perform(World*, Entity*) const
	{}

	void MovementAction::perform(World *world, Entity *target) const
	{
		if(world->get_map() != nullptr)
		{
			auto target_tile = world->get_map()->at(target->pos + Pos(dx, dy));
			if(target_tile->walkable)
			{
				target->move(dx, dy);
				if(world->get_player() == target)
					world->get_map()->update_fov(target);
			}
		}
		else
			target->move(dx, dy);
	}

	void VisibilityAction::perform(World *world, Entity *target) const
	{
		if(world->get_map() != nullptr)
		{
			switch (viz_type)
			{
			case REVEAL: world->get_map()->flood_reveal(target->pos); break;
			case HIDE:
				world->get_map()->amnesia();
				if(world->get_player() == target)
					world->get_map()->update_fov(target);
				break;
			case OMNIPOTENT: world->get_map()->nightmare_eyes(); break;
			}
		}
	}

}