#pragma once

#include <logging.h>

#include <cstddef>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace GarrysMod::Lua
{
	class ILuaBase;
}

namespace logging::listener
{
	class Message
	{
	public:
		Message( const LoggingSeverity_t severity, const LoggingChannelID_t group, const Color &color, const LoggingChannelFlags_t flags, const char *msg );

		void ToLua( GarrysMod::Lua::ILuaBase *LUA ) const;

	private:
		double m_time;
		LoggingSeverity_t m_severity;
		LoggingChannelID_t m_channel;
		Color m_color;
		LoggingChannelFlags_t m_flags;
		std::string m_msg;
	};

	class LoggingListenerAndResponsePolicy : public ILoggingListener
	{
	public:
		std::vector<Message> PopMessages( const size_t num = -1 );

		int32_t PushMessages( GarrysMod::Lua::ILuaBase *LUA, const size_t num = -1 );

		void SetMaximumQueueSize( const size_t max_queue_size );

		size_t GetMaximumQueueSize( ) const;

	private:
		virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage ) override;

		std::mutex m_mutex;
		std::queue<Message> m_messages;
		size_t m_max_messages = 1000;
	};
}
