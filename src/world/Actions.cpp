#include "Actions.hpp"

#include "World.hpp"
#include "Constants.hpp"

namespace RYSIC::World
{

	Action* GetActionFromEvent(SDL_Event &event, World* world)
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
				return new BumpAction({0, -1}); // up
			case SDLK_DOWN:
			case SDLK_KP_2:
			case SDLK_k:
				return new BumpAction({0, 1}); // down
			case SDLK_LEFT:
			case SDLK_KP_4:
			case SDLK_h:
				return new BumpAction({-1, 0}); // left
			case SDLK_RIGHT:
			case SDLK_KP_6:
			case SDLK_l:
				return new BumpAction({1, 0}); // right
			case SDLK_KP_7:
				return new BumpAction({-1, -1}); // up-left
			case SDLK_KP_9:
				return new BumpAction({1, -1}); // up-right
			case SDLK_KP_1:
				return new BumpAction({-1, 1}); // down-left
			case SDLK_KP_3:
				return new BumpAction({1, 1}); // down-right
			case SDLK_PERIOD:
			case SDLK_KP_5:
				return new BumpAction({0, 0}); // move in place (wait a turn)
			case SDLK_r:
				return new VisibilityAction(VisibilityAction::OMNIPOTENT); // reveal 
			case SDLK_f:
				return new VisibilityAction(VisibilityAction::HIDE); // hide 
			default:
				char buf[32];
				sprintf_s(buf, Constants::MSG_NO_ACTION_FOR_KEY, SDL_GetKeyName(event.key.keysym.sym));
				world->message(buf, Log::MessageType::SYSTEM);
				break;
			}
		}
		return nullptr;
	}

	DirectionalAction::Direction DirectionalAction::dir() const
	{
		if(target_pos.x == 0 && target_pos.y == 0)
			return Direction::NONE;
		if(target_pos.y < 0)
		{
			if(target_pos.x < 0)
				return Direction::NORTHWEST;
			else if(target_pos.x > 0)
				return Direction::NORTHEAST;
			else
				return Direction::NORTH;
		}
		else
		{
			if(target_pos.x < 0)
				return Direction::SOUTHWEST;
			else if(target_pos.x > 0)
				return Direction::SOUTHEAST;
			else
				return Direction::SOUTH;
		}
	}

	void MovementAction::perform(World *world, Entity *target) const
	{
		if(world->get_map())
		{
			Pos dest = target->pos + target_pos;
			auto target_tile = world->get_map()->at(dest);
			auto target_blocking_entity = world->get_map()->blocking_entity_at(dest);
			if(!target_tile->walkable)
				return;
			if(target_blocking_entity)
				return;
			target->move(target_pos);
			world->progress(target->stats.move_speed);
			if(world->get_player() == target)
				world->get_map()->update_fov(target);
		}
		else
			target->move(target_pos);
	}

	void MeleeAction::perform(World *world, Entity *target) const
	{
		Pos dest = target->pos + target_pos;
		if(world->get_map())
		{
			auto target_entity = world->get_map()->blocking_entity_at(dest);
			if(!target_entity)
				return;
			printf("You bump against %s, with little effect.\n", target_entity->name.c_str());
		}
	}

	void VisibilityAction::perform(World *world, Entity *target) const
	{
		if(world->get_map())
		{
			switch (viz_type)
			{
			case REVEAL: world->get_map()->flood_reveal(target->pos); break;
			case HIDE:
				world->get_map()->amnesia();
				if(world->get_player() == target)
					world->get_map()->update_fov(target);
				break;
			case OMNIPOTENT: world->get_map()->nightmare_eyes(target->pos); break;
			}
		}
	}

	void BumpAction::perform(World *world, Entity *target) const
	{
		Pos dest = target->pos + target_pos;
		if(world->get_map())
		{
			if(world->get_map()->blocking_entity_at(dest))
				MeleeAction(target_pos).perform(world, target);
			else
				MovementAction(target_pos).perform(world, target);
		}
	}

}