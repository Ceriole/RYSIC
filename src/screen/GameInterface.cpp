#include "GameInterface.hpp"

#include "Constants.hpp"
#include "ScreenUtil.hpp"
#include "Game.hpp"
#include "world/entity/Actor.hpp"
#include "world/Map.hpp"

namespace RYSIC::Interface
{
	CharacterPanel::CharacterPanel(Rect _rect, World::World *world)
		: Container(_rect), m_world(world), m_actor(nullptr)
	{

		int iw = rect.w - 2;
		int y = 0;

		m_lbl_time = Ref<Label>(new Label({0, y++, rect.w, 1}, "???", std::nullopt, std::nullopt));
		add(m_lbl_time);
		m_lbl_name = Ref<Label>(new Label({1, y++, iw, 1}, "???", C_WHITE, std::nullopt));
		m_lbl_name->align = HorizontalAlignment::HA_CENTER;

		m_lbl_description = Ref<Label>(new Label({1, y, iw, 10}, "?? / ??", std::nullopt, std::nullopt));
		add(m_lbl_description);

		add(m_lbl_name);
		m_lbl_level = Ref<Label>(new Label({1, y++, iw, 1}, "Level ??", std::nullopt, std::nullopt));
		add(m_lbl_level);
		m_bar_xp = Ref<ProgressBar>(new ProgressBar({1, y++}, iw, "\u4002", std::nullopt, C_BLUE, C_DARK_BLUE));
		m_bar_xp->align = HorizontalAlignment::HA_RIGHT;
		add(m_bar_xp);
		m_lbl_xp = Ref<Label>(new Label({1, y++, iw, 1}, "?? / ??", std::nullopt, std::nullopt));
		m_lbl_xp->align = HorizontalAlignment::HA_RIGHT;
		add(m_lbl_xp);

		m_bar_health = Ref<ProgressBar>(new ProgressBar({1, y++}, iw, "\u4001", std::nullopt, C_RED, C_DARK_RED));
		m_bar_health->align = HorizontalAlignment::HA_RIGHT;
		add(m_bar_health);
		m_lbl_health = Ref<Label>(new Label({1, y++, iw, 1}, "?? / ??", std::nullopt, std::nullopt));
		m_lbl_health->align = HorizontalAlignment::HA_RIGHT;
		add(m_lbl_health);

		y++;

		for(int i = 0; i < 3; i++)
		{
			m_lbl_stat[i] = Ref<Label>(new Label({1, y++, iw, 10}, "STAT ??", std::nullopt, std::nullopt));
			add(m_lbl_stat[i]);
		}
	}

	void CharacterPanel::render(TCOD_Console &console)
	{
		m_lbl_time->text = m_world->time()->string();
		m_lbl_name->enabled = false;
		m_lbl_level->enabled = false;
		m_bar_xp->enabled = false;
		m_lbl_xp->enabled = false;
		m_bar_health->enabled = false;
		m_lbl_health->enabled = false;
		m_lbl_description->enabled = false;
		for(int i = 0; i < 3; i++)
			m_lbl_stat[i]->enabled = false;
		switch(m_mode)
		{
		case ACTOR:
			m_lbl_name->text = m_actor->name;
			m_lbl_name->enabled = true;
			m_lbl_level->text = fmt::format("Level {}", m_actor->stats.level());
			m_lbl_level->enabled = true;
			m_bar_xp->set(m_actor->stats.xp_ratio());
			m_bar_xp->enabled = m_actor->is_player();
			m_lbl_xp->text = fmt::format("{} / {}", m_actor->stats.xp(), m_actor->stats.xp_to_next());
			m_lbl_xp->enabled = m_actor->is_player();

			m_bar_health->set(m_actor->stats.hp_ratio());
			m_bar_health->enabled = true;
			m_lbl_health->text = fmt::format("{} / {}", m_actor->stats.hp(), m_actor->stats.max_hp());
			m_lbl_health->enabled = true;
			
			m_lbl_stat[0]->text = fmt::format("TEN {}", m_actor->stats.ten());
			if(m_actor->stats.ten_mod() != 0)
				m_lbl_stat[0]->text.append(fmt::format(" ({:+})", m_actor->stats.ten_mod()));
			m_lbl_stat[0]->fg = GetStatColor(m_actor->stats.ten_mod());
			
			m_lbl_stat[1]->text = fmt::format("SAG {}", m_actor->stats.sag());
			if(m_actor->stats.sag_mod() != 0)
				m_lbl_stat[1]->text.append(fmt::format(" ({:+})", m_actor->stats.sag_mod()));
			
			m_lbl_stat[2]->text = fmt::format("VIV {}", m_actor->stats.viv());
			if(m_actor->stats.viv_mod() != 0)
				m_lbl_stat[2]->text.append(fmt::format(" ({:+})", m_actor->stats.viv_mod()));
			m_lbl_stat[2]->fg = GetStatColor(m_actor->stats.viv_mod());
			
			for(int i = 0; i < 3; i++)
				m_lbl_stat[i]->enabled = true;
			
			break;
		case DESCRIPTION:
			m_lbl_name->text = m_title;
			m_lbl_name->enabled = true;
			m_lbl_description->text = m_description;
			m_lbl_description->enabled = true;
			break;
		case UNSEEN:
			m_lbl_description->text = "Unseen.";
			m_lbl_description->enabled = true;
			break;
		default:
		case NONE:
			break;
		}

		Container::render(console);
	}

	void LogPanel::render(TCOD_Console &console)
	{
		if(!enabled)
			return;

		tcod::Console subconsole = Util::Screen::CreateComponentCanvas(rect.w, rect.h, fg, bg);
		if(m_log)
		{
			tcod::Console message_box = Util::Screen::CreateComponentCanvas(rect.w, rect.h - 1, fg, bg);
			int y = rect.h - 1;
			for(auto msg = m_log->end(); msg != m_log->begin();)
			{
				--msg;
				if(msg->age() >= Constants::MESSAGE_AGE_OLD)
					continue;
				y -= tcod::get_height_rect(rect.w, msg->str());
				tcod::print_rect(message_box, {0, y, rect.w, 0}, msg->str(), msg->fg_color(fg), msg->bg_color(bg));
				if(y < 1)
					break;
			}
			tcod::blit(subconsole, message_box, {0, 1});
		}
		else
		{
			tcod::print_rect(subconsole, {0, rect.h / 2, rect.w, 0}, "Null.", std::nullopt, std::nullopt, TCOD_CENTER);
		}
		

		Util::Screen::draw_hline(subconsole, 0, 0, 0, 0x2550, std::nullopt, std::nullopt);
		tcod::print_rect(subconsole, {0, 0, rect.w, 1}, "\u2561MESSAGE LOG\u255e", std::nullopt, std::nullopt, TCOD_CENTER);

		tcod::blit(console, subconsole, {rect.x, rect.y});
	}

}