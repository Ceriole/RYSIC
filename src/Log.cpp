#include "Log.hpp"

namespace RYSIC::Log
{
	
	void MessageFeed::message(const Message &msg)
	{
		if(m_log.size() > 0)
		{
			Message &last = m_log.back();
			if(last == msg)
			{
				last.count++;
				return;
			}
		}
		m_log.push_back(msg);
	}

}