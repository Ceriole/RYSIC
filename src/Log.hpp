#pragma once

#include <string>
#include <vector>

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

		bool operator==(const Message &lhs) const
		{ return !msg.compare(lhs.msg) && type == lhs.type; }
	};

	class MessageFeed
	{
	private:
		std::vector<Message> m_log;
	public:
		void message(const Message &msg);
	};

}