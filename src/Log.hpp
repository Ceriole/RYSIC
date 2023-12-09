#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

namespace RYSIC::Log
{

	enum MessageType
	{
		NORMAL = 0, GOOD, BAD, GREAT, TERRIBLE, SPECIAL, SYSTEM
	};

	struct Message
	{
		std::string msg;
		MessageType type;
		unsigned long timestamp;
		unsigned int count = 0;

		const std::string str() const;
		const Color fg_color(const Color& base_fg = std::nullopt) const;
		const Color bg_color(const Color& base_bg = std::nullopt) const;

		bool operator==(const Message &lhs) const
		{ return !msg.compare(lhs.msg) && type == lhs.type; }

		unsigned long age() const;
	};

	class MessageFeed
	{
	private:
		std::vector<Message> m_complete_log;
		std::vector<Message> m_log;
	public:
		void message(const Message &msg);
		void clear();
		void nuke();

		size_t count() const { return m_log.size(); };
		size_t total_count() const { return m_complete_log.size(); };

		auto begin()
		{ return m_log.begin(); }
		auto end()
		{ return m_log.end(); }

		auto rbegin()
		{ return m_log.rend(); }
		auto rend()
		{ return m_log.rbegin(); }
	};

}