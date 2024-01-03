#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include "AI.hpp"
#include "Constants.hpp"

namespace RYSIC::World
{

	class Actor : public Entity
	{
	public:
		AI::BaseAI* ai;
		StatComponent stats;
		int defeat_xp;

		Actor(Map* map, const Pos &_pos, const Glyph &_gfx, const std::string& _name, int _defeat_xp = 0, int ten = Constants::STAT_BASE_VALUE, int sag = Constants::STAT_BASE_VALUE, int viv = Constants::STAT_BASE_VALUE)
			: Entity(map, _pos, _gfx, _name, true, RenderOrder::ACTOR), stats(this, ten, sag, viv), defeat_xp(_defeat_xp)
		{}

		virtual void debug_render(TCOD_Console &console) const override;

		void progress(unsigned long tics) override { if(thinks()) ai->progress(tics); };
		
		template<class AI_class>
		Actor* set_ai() { ai = new AI_class(this); return this; }
		virtual bool thinks() const override { return ai; };

		void melee_attack(Actor* target);

		bool is_player() const;

		void die();
	};

}