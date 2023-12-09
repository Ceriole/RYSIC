#include "Log.hpp"

namespace RYSIC::Log
{

	const std::string Message::str() const
	{
		char buf[6];
		sprintf_s(buf, "(x%d)", MIN(count + 1, 99));
		return msg + (count > 0 ? buf : "");
	}

	const Color Message::fg_color() const
	{
		Color fg = std::nullopt;
		switch (type)
		{
		default:
		case MessageType::NORMAL:
			fg = std::nullopt;	break;
		case MessageType::GOOD:
			fg = {{20, 20, 170}};	break;
		case MessageType::BAD:
			fg = {{170, 20, 20}};	break;
		case MessageType::GREAT:
			fg = {{170, 255, 0}};	break;
		case MessageType::TERRIBLE:
			fg = {{255, 170, 0}};	break;
		case MessageType::SYSTEM:
			fg = {{0, 150, 70}};	break;
		}

		return fg;
	}

	const Color Message::bg_color() const
	{
		Color bg = std::nullopt;
		switch (type)
		{
		default:
		case MessageType::NORMAL:
		case MessageType::GOOD:
		case MessageType::BAD:
			bg = std::nullopt;	break;
		case MessageType::GREAT:
			bg = {{5, 50, 0}};	break;
		case MessageType::TERRIBLE:
			bg = {{50, 0, 0}};	break;
		case MessageType::SYSTEM:
			bg = {{0, 75, 50}};	break;
		}

		return bg;
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
