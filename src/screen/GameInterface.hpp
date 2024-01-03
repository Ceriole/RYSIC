#pragma once
#include "Interface.hpp"

namespace RYSIC
{
	namespace World
	{
		class World;
		class Actor;
	}

	namespace Interface
	{

		constexpr Color GetStatColor(int value)
		{
			if(value > 0)
				return C_GREEN;
			else if(value < 0)
				return C_RED;
			else
				return std::nullopt;
		}

		class CharacterPanel : public Container
		{
		private:
			enum Mode { NONE, ENITIES, ACTOR, DESCRIPTION, UNSEEN } m_mode;
			World::World* m_world;
			World::Actor* m_actor;
			std::string m_title, m_description;

			Ref<Label> m_lbl_time;

			Ref<Label> m_lbl_name;
			Ref<Label> m_lbl_level;
			Ref<ProgressBar> m_bar_xp;
			Ref<Label> m_lbl_xp;

			Ref<ProgressBar> m_bar_health;
			Ref<Label> m_lbl_health;

			Ref<Label> m_lbl_description;

			Ref<Label> m_lbl_stat[3];

		public:
			CharacterPanel() = default;
			CharacterPanel(Rect _rect, World::World *world);

			virtual void render(TCOD_Console &console) override;

			void clear() { m_actor = nullptr; m_title = ""; m_description = ""; m_mode = NONE; };
			void set_unseen() { clear(); m_mode = UNSEEN; };
			void set_character(World::Actor* actor) { clear(); m_actor = actor; m_mode = m_actor ? ACTOR : NONE; };
			void set_description(std::string_view title, std::string_view description = "")
			{ clear(); m_title = title; m_description = description; m_mode = DESCRIPTION; }
		};

		class LogPanel : public Component
		{
		public:
			Color fg, bg;
		private:
			Log::MessageFeed* m_log;

		public:
			LogPanel() = default;
			LogPanel(const Rect& _rect, Log::MessageFeed* log)
				: Component(_rect), m_log(log)
			{}

			virtual void render(TCOD_Console &console) override;
		};
	}
}