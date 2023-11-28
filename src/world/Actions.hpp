#pragma once

#include <SDL2/SDL.h>

namespace RYSIC::World
{

	class World;
	class Entity;

	struct Action
	{
		enum Type
		{
			NONE, EXIT, MOVEMENT
		};

		virtual Type type() const { return NONE; };

		virtual void perform(World* world, Entity* target) const;
	};

	struct ExitAction : public Action
	{
		virtual Type type() const override { return EXIT; };
	};

	struct MovementAction : public Action
	{
		const int dx, dy;
		enum Direction
		{
			NONE, NORTH, SOUTH, EAST, WEST,
			NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST
		};

		MovementAction(int _dx, int _dy)
			: dx(_dx), dy(_dy)
		{}

		virtual Type type() const override { return MOVEMENT; };
		Direction dir() const;

		virtual void perform(World* world, Entity* target) const override;
	};
	
	Action* GetActionFromEvent(SDL_Event &event);
}