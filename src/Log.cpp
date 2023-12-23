#include "Log.hpp"

#include "world/World.hpp"
#include "Constants.hpp"
#include "Game.hpp"

#include <fmt/format.h>

namespace RYSIC::Log
{

	const std::string Message::str() const
	{
		return msg + (count > 0 ? fmt::format("(x{})", MIN(count + 1, 99)) : "");
	}

	const Color Message::fg_color(const Color& base_fg) const
	{
		Color fg = std::nullopt;
		switch (type)
		{
		default:
		case MessageType::NORMAL:
			fg = base_fg;	break;
		case MessageType::GOOD:
			fg = C_GREEN;	break;
		case MessageType::BAD:
			fg = C_DARK_RED;	break;
		case MessageType::GREAT:
			fg = C_LIGHT_GREEN;	break;
		case MessageType::TERRIBLE:
			fg = C_DARK_MAGENTA;	break;
		case MessageType::SYSTEM:
			fg = C_ORANGE;	break;
		}

		if(age() >= Constants::MESSAGE_AGE_STALE)
			if(fg.has_value())
				fg = TCOD_color_lerp(fg.value(), {}, 0.5f);

		return fg;
	}

	const Color Message::bg_color(const Color& base_bg) const
	{
		Color bg = std::nullopt;
		switch (type)
		{
		default:
		case MessageType::NORMAL:
		case MessageType::GOOD:
		case MessageType::BAD:
		case MessageType::GREAT:
		case MessageType::TERRIBLE:
		case MessageType::SYSTEM:
			bg = base_bg;	break;
		}

		if(age() >= Constants::MESSAGE_AGE_STALE)
			if(bg.has_value())
				bg = TCOD_color_lerp(bg.value(), base_bg.value_or(TCOD_ColorRGB{}), 0.5f);

		return bg;
	}

	unsigned long Message::age() const
	{
		if(Game::Instance()->world())
			return Game::Instance()->world()->time() - timestamp;
		else
			return timestamp;
	}

	void MessageFeed::message(const Message &msg)
	{
		m_complete_log.push_back(msg);
		if(m_log.size() > 0)
		{
			Message &last = m_log.back();
			if(last == msg)
			{
				last.count++;
				last.timestamp = msg.timestamp;
				return;
			}
		}
		m_log.push_back(msg);
	}

	void MessageFeed::clear()
	{
		m_log.clear();
	}

	void MessageFeed::nuke()
	{
		m_complete_log.clear();
		clear();
	}
	
}
