#pragma once

#include <SDL2/SDL.h>

#include "Types.hpp"

namespace RYSIC::World
{

	class World;
	class Entity;

	struct Action
	{
		enum Type
		{
			EXIT, DIRECTIONAL, MOVEMENT, ATTACK, VISIBILITY
		};

		virtual Type type() const = 0;

		virtual void perform(World* world, Entity* target) const = 0;
	};

	struct ExitAction : virtual public Action
	{
		virtual Type type() const override { return EXIT; };

		virtual void perform(World*, Entity*) const {};
	};

	struct DirectionalAction : virtual public Action
	{
		const Pos target_pos;
		enum Direction
		{
			NONE, NORTH, SOUTH, EAST, WEST,
			NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST
		};

		DirectionalAction(const Pos& _target_pos)
			: target_pos(_target_pos)
		{}

		virtual Type type() const = 0;
		Direction dir() const;

		virtual void perform(World* world, Entity* target) const = 0;
	};

	struct BumpAction : virtual public DirectionalAction
	{

		BumpAction(const Pos& _target_pos)
			: DirectionalAction(_target_pos)
		{}

		virtual Type type() const override { return ATTACK; };

		virtual void perform(World* world, Entity* target) const override;
	};

	struct MovementAction : virtual public DirectionalAction
	{

		MovementAction(const Pos& _target_pos)
			: DirectionalAction(_target_pos)
		{}

		virtual Type type() const override { return MOVEMENT; };

		virtual void perform(World* world, Entity* target) const override;
	};

	struct MeleeAction : virtual public DirectionalAction
	{

		MeleeAction(const Pos& _target_pos)
			: DirectionalAction(_target_pos)
		{}

		virtual Type type() const override { return ATTACK; };

		virtual void perform(World* world, Entity* target) const override;
	};

	struct VisibilityAction : virtual public Action
	{
		const enum VisType { REVEAL, HIDE, OMNIPOTENT } viz_type;

		VisibilityAction(VisType _viz_type = REVEAL)
			: viz_type(_viz_type)
		{}

		virtual Type type() const override { return VISIBILITY; };

		virtual void perform(World* world, Entity* target) const override;
	};
	
	Action* GetActionFromEvent(SDL_Event &event, World* world);
}