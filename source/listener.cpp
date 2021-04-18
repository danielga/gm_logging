#include "listener.hpp"
#include "helpers.hpp"

#include <GarrysMod/Lua/Interface.h>

#include <logging.h>

#include <cstddef>
#include <vector>
#include <mutex>

namespace logging::listener
{
	using namespace logging::helpers;

	Message::Message( const LoggingSeverity_t severity, const LoggingChannelID_t group, const Color &color, const LoggingChannelFlags_t flags, const char *msg ) :
		m_time( Plat_FloatTime( ) ),
		m_severity( severity ),
		m_channel( group ),
		m_color( color ),
		m_flags( flags ),
		m_msg( msg )
	{ }

	void Message::ToLua( GarrysMod::Lua::ILuaBase *LUA ) const
	{
		LUA->CreateTable( );

			LUA->PushNumber( m_time );
			LUA->SetField( -2, "Timestamp" );

			LUA->PushNumber( m_severity );
			LUA->SetField( -2, "Severity" );

			LUA->PushNumber( m_channel );
			LUA->SetField( -2, "ChannelID" );

			ColorToLua( LUA, m_color );
			LUA->SetField( -2, "Color" );

			LUA->PushNumber( m_flags );
			LUA->SetField( -2, "Flags" );

			LUA->PushString( m_msg.c_str( ) );
			LUA->SetField( -2, "Message" );
	}

	std::vector<Message> LoggingListenerAndResponsePolicy::PopMessages( const size_t num )
	{
		size_t count = num;
		const size_t size = m_messages.size( );
		if( count > size )
			count = size;

		std::vector<Message> messages;
		if( count == 0 )
			return messages;

		messages.reserve( count );

		std::lock_guard<std::mutex> lock( m_mutex );
		for( size_t k = 0; k < count; ++k )
		{
			messages.emplace_back( m_messages.front( ) );
			m_messages.pop( );
		}

		return messages;
	}

	int32_t LoggingListenerAndResponsePolicy::PushMessages( GarrysMod::Lua::ILuaBase *LUA, const size_t num )
	{
		const std::vector<Message> messages = PopMessages( num );
		if( messages.empty( ) )
			return 0;

		LUA->CreateTable( );

		size_t k = 0;
		for( const auto &message : messages )
		{
			LUA->PushNumber( ++k );

			message.ToLua( LUA );

			LUA->SetTable( -3 );
		}

		return 1;
	}

	void LoggingListenerAndResponsePolicy::SetMaximumQueueSize( const size_t max_queue_size )
	{
		m_max_messages = max_queue_size;
	}

	size_t LoggingListenerAndResponsePolicy::GetMaximumQueueSize( ) const
	{
		return m_max_messages;
	}

	void LoggingListenerAndResponsePolicy::Log( const LoggingContext_t *pContext, const tchar *pMessage )
	{
		Message message(
			pContext->m_Severity,
			pContext->m_ChannelID,
			pContext->m_Color,
			pContext->m_Flags,
			pMessage
		);

		std::lock_guard<std::mutex> lock( m_mutex );
		if( m_messages.size( ) >= m_max_messages )
			m_messages.pop( );

		m_messages.emplace( std::move( message ) );
	}
}
