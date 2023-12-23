#pragma once

#include <SDL2/SDL.h>

namespace RYSIC
{
	class Game;
	class Action;
	namespace World
	{
		class World;
	}
	
	class EventHandler
	{
	protected:
		Game* m_game;
		bool m_shift, m_ctrl, m_alt;
		
	public:
		EventHandler(Game* game)
			: m_game(game)
		{}
		~EventHandler()
		{}
		
		
		virtual void handle(SDL_Event &event);

	protected:
		virtual Action* get_action_from_SDL_event(SDL_Event &event);
	
	private:
		bool handle_modifier_keys(SDL_Event &event);
	};

	class GameplayEventHandler : public virtual EventHandler
	{
	protected:
		World::World* m_world;
	public:
		GameplayEventHandler(Game* game, World::World* world)
			: EventHandler(game), m_world(world)
		{}

	protected:
		virtual Action* get_action_from_SDL_event(SDL_Event &event) override;
	};

	class GameOverEventHandler : public virtual EventHandler
	{
	public:
		GameOverEventHandler(Game* game)
			: EventHandler(game)
		{}
	};

}