#include "listener.hpp"
#include "channel.hpp"
#include "helpers.hpp"

#include <GarrysMod/Lua/Interface.h>

#include <logging.h>

#include <cstdint>
#include <queue>
#include <mutex>
#include <limits>

namespace logging
{
	using namespace logging::listener;
	using namespace logging::helpers;

	static LoggingListenerAndResponsePolicy s_logger;

	LUA_FUNCTION_STATIC( Get )
	{
		size_t count = 1;
		if( LUA->Top( ) != 0 )
			count = GetNumberFromLua<size_t>( LUA, 1 );

		return s_logger.PushMessages( LUA, count );
	}

	LUA_FUNCTION_STATIC( GetAll )
	{
		return s_logger.PushMessages( LUA );
	}

	LUA_FUNCTION_STATIC( SetMaximumQueueSize )
	{
		const auto max_queue_size = GetNumberFromLua<size_t>( LUA, 1 );
		s_logger.SetMaximumQueueSize( max_queue_size );
		return 0;
	}

	LUA_FUNCTION_STATIC( GetMaximumQueueSize )
	{
		LUA->PushNumber( static_cast<double>( s_logger.GetMaximumQueueSize( ) ) );
		return 1;
	}

	static void Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LoggingSystem_RegisterLoggingListener( &s_logger );

		LUA->PushString( "logging 1.0.0" );
		LUA->SetField( -2, "Version" );

		// version num follows LuaJIT style, xxyyzz
		LUA->PushNumber( 10000 );
		LUA->SetField( -2, "VersionNum" );

		LUA->PushCFunction( Get );
		LUA->SetField( -2, "Get" );

		LUA->PushCFunction( GetAll );
		LUA->SetField( -2, "GetAll" );

		LUA->PushCFunction( SetMaximumQueueSize );
		LUA->SetField( -2, "SetMaximumQueueSize" );

		LUA->PushCFunction( GetMaximumQueueSize );
		LUA->SetField( -2, "GetMaximumQueueSize" );
	}

	static void Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LoggingSystem_UnregisterLoggingListener( &s_logger );
	}
}

GMOD_MODULE_OPEN( )
{
	LUA->CreateTable( );

	logging::Initialize( LUA );
	logging::channel::Initialize( LUA );

	LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "logging" );

	return 0;
}

GMOD_MODULE_CLOSE( )
{
	logging::Deinitialize( LUA );

	LUA->PushNil( );
	LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "logging" );

	return 0;
}
