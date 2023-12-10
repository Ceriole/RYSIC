#pragma once

#include <vector>

#include "Actions.hpp"
#include "Component.hpp"

namespace RYSIC::World
{
	class Actor;

	namespace AI
	{

		class BaseAI : public Action, public BaseComponent
		{
		protected:
			struct ActorAction {
				Action* current = nullptr;
				unsigned long delay = 0;
			} action;
		public:
			BaseAI(Actor* actor)
				: BaseComponent(actor)
			{}
			
			void progress(unsigned long tics);

			template<class ActionClass>
			bool is_action() { if(!action.current) return false; return dynamic_cast<ActionClass*>(action.current); }

			virtual void perform() override {};
			std::vector<Pos> get_path_to(const Pos& dest) const;
		protected:
			bool start_action(Action* new_action, bool priority = false);

			const class PathCallback : public ITCODPathCallback
			{
			public:
				virtual float getWalkCost(int ox, int oy, int dx, int dy, void* data) const override;
			} path_callback;
		};
		
		class HostileAI : public BaseAI
		{
		private:
			std::vector<Pos> m_path;
		public:
			HostileAI(Actor* actor)
				: BaseAI(actor)
			{}
			
			virtual void perform() override;
		};

	}
}
