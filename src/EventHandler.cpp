#include "EventHandler.hpp"

#include "Actions.hpp"
#include "Constants.hpp"
#include "Log.hpp"
#include "world/World.hpp"
#include "world/entity/Actor.hpp"

namespace RYSIC
{
	
	void EventHandler::handle(SDL_Event &event)
	{
		Action* action = get_action_from_SDL_event(event);
		if(!action)
			return;
		
		action->perform();
	}

	Action *EventHandler::get_action_from_SDL_event(SDL_Event &event)
	{
		if(event.type == SDL_QUIT)
			return new ExitAction(m_game);
		if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return new ExitAction(m_game); // quit
			}
		}
		return nullptr;
	}

	Action *GameplayEventHandler::get_action_from_SDL_event(SDL_Event &event)
	{
		if(Action* action = EventHandler::get_action_from_SDL_event(event); action)
			return action;
		if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_KP_8:
			case SDLK_j:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {0, -1}); // up
			case SDLK_DOWN:
			case SDLK_KP_2:
			case SDLK_k:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {0, 1}); // down
			case SDLK_LEFT:
			case SDLK_KP_4:
			case SDLK_h:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {-1, 0}); // left
			case SDLK_RIGHT:
			case SDLK_KP_6:
			case SDLK_l:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {1, 0}); // right
			case SDLK_KP_7:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {-1, -1}); // up-left
			case SDLK_KP_9:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {1, -1}); // up-right
			case SDLK_KP_1:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {-1, 1}); // down-left
			case SDLK_KP_3:
				return new BumpAction(m_world, (World::Entity*) m_world->player(), {1, 1}); // down-right
			case SDLK_PERIOD:
			case SDLK_KP_5:
				return new WaitAction(m_world, (World::Entity*) m_world->player(), Constants::WAIT_LENGTH); // move in place (wait a turn)
			case SDLK_r:
				return new VisibilityAction(m_world, (World::Entity*) m_world->player(), VisibilityAction::OMNIPOTENT); // reveal 
			case SDLK_f:
				return new VisibilityAction(m_world, (World::Entity*) m_world->player(), VisibilityAction::HIDE); // hide 
			default:
				m_world->message(tcod::stringf(Constants::MSG_NO_ACTION_FOR_KEY, SDL_GetKeyName(event.key.keysym.sym)), Log::MessageType::SYSTEM);
				break;
			}
		}
		return nullptr;
	}
}