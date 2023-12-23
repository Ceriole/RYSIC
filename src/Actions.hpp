#pragma once

#include <SDL2/SDL.h>

#include "Types.hpp"

namespace RYSIC
{
	class Game;

	namespace World
	{
		class World;
		class Entity;
		class Actor;
	}

	class Action
	{
	public:
		virtual void perform() {};
	};

	class SystemAction : public Action
	{
	protected:
		Game* m_game;
	public:
		SystemAction(Game* game)
			: m_game(game)
		{}

		virtual void perform() override {}
	};

	class ExitAction : public SystemAction
	{
	public:
		ExitAction(Game* game)
			: SystemAction(game)
		{}

		virtual void perform() override;
	};

	class FullscreenToggleAction : public SystemAction
	{
	public:
		FullscreenToggleAction(Game* game)
			: SystemAction(game)
		{}

		virtual void perform() override;
	};

	class DebugToggleAction : public SystemAction
	{
	public:
		DebugToggleAction(Game* game)
			: SystemAction(game)
		{}

		virtual void perform() override;
	};

	class WorldAction : public Action
	{
	protected:
		World::World* m_world;
	public:
		WorldAction(World::World* world)
			: m_world(world)
		{}

		virtual void perform() override {}
	};

	class EntityAction : public WorldAction
	{
	protected:
		World::Entity* m_entity;
	public:
		EntityAction(World::World* world, World::Entity* entity)
			: WorldAction(world), m_entity(entity)
		{}

		virtual void perform() override {}
	};

	class WaitAction : public EntityAction
	{
	public:
		const unsigned long tics;

		WaitAction(World::World* world, World::Entity* entity, unsigned long _tics = 0)
			: EntityAction(world, entity), tics(_tics)
		{}

		virtual void perform() override;
	};

	class DirectionalAction : public EntityAction
	{
	protected:
		const Pos m_target;
		enum Direction
		{
			NONE, NORTH, SOUTH, EAST, WEST,
			NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST
		};

	public:
		DirectionalAction(World::World* world, World::Entity* entity, const Pos& target)
			: EntityAction(world, entity), m_target(target)
		{}

		Direction dir() const;
		const Pos destination() const;
		World::Entity *get_blocking_entity() const;
		World::Actor *get_actor() const;

		virtual void perform() override {}
	};

	class BumpAction : public DirectionalAction
	{
	public:
		BumpAction(World::World* world, World::Entity* entity, const Pos& target)
			: DirectionalAction(world, entity, target)
		{}

		virtual void perform() override;
	};

	class MovementAction : public DirectionalAction
	{
	public:
		MovementAction(World::World* world, World::Entity* entity, const Pos& target)
			: DirectionalAction(world, entity, target)
		{}

		virtual void perform() override;
	};

	class MeleeAction : public DirectionalAction
	{
	public:
		MeleeAction(World::World* world, World::Entity* entity, const Pos& target)
			: DirectionalAction(world, entity, target)
		{}

		virtual void perform() override;
	};

	class VisibilityAction : public EntityAction
	{
	public:
		const enum VisType { REVEAL, HIDE, OMNIPOTENT } viz_type;
	public:
		VisibilityAction(World::World* world, World::Entity* entity, VisType viz_type = VisType::REVEAL)
			: EntityAction(world, entity), viz_type(viz_type)
		{}

		virtual void perform() override;
	};
}