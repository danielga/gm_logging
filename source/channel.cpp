#include "channel.hpp"
#include "helpers.hpp"

#include <GarrysMod/Lua/Interface.h>

#include <logging.h>

#include <cstdint>

namespace logging::channel
{
	using namespace logging::helpers;

#pragma region Helpers

	static LoggingSeverity_t GetSeverityFromLua( GarrysMod::Lua::ILuaBase *LUA, const int32_t index )
	{
		return GetNumberFromLua<LoggingSeverity_t>( LUA, index, "invalid logging severity", LS_MESSAGE, LS_ERROR );
	}

#pragma endregion Helpers

#pragma region MetaTable

	static const char *metaname = "logging::channel";
	static int32_t metatype = -1;
	static const char *tostring_format = "%s: %p";

	inline void Push( GarrysMod::Lua::ILuaBase *LUA, LoggingChannelID_t channelID )
	{
		if( channelID == INVALID_LOGGING_CHANNEL_ID )
		{
			LUA->PushNil( );
			return;
		}

		const auto channel = const_cast<CLoggingSystem::LoggingChannel_t *>( LoggingSystem_GetChannel( channelID ) );

		LUA->PushUserType( channel, metatype );

		LUA->PushMetaTable( metatype );
		LUA->SetMetaTable( -2 );

		LUA->CreateTable( );
		LUA->SetFEnv( -2 );
	}

	inline void CheckType( GarrysMod::Lua::ILuaBase *LUA, const int32_t index )
	{
		if( !LUA->IsType( index, metatype ) )
			LUA->TypeError( index, metaname );
	}

	inline const CLoggingSystem::LoggingChannel_t *GetUserData( GarrysMod::Lua::ILuaBase *LUA, const int32_t index, const bool check_type = true )
	{
		if( check_type )
			CheckType( LUA, index );

		return LUA->GetUserType<const CLoggingSystem::LoggingChannel_t>( index, metatype );
	}

	inline const CLoggingSystem::LoggingChannel_t *Get( GarrysMod::Lua::ILuaBase *LUA, const int32_t index )
	{
		const auto channel = GetUserData( LUA, index );
		if( channel == nullptr )
			LUA->FormattedError( "invalid %s", metaname );

		return channel;
	}

	LUA_FUNCTION( index )
	{
		CheckType( LUA, 1 );

		LUA->GetMetaTable( 1 );
		LUA->Push( 2 );
		LUA->RawGet( -2 );
		if( !LUA->IsType( -1, GarrysMod::Lua::Type::NIL ) )
			return 1;

		LUA->GetFEnv( 1 );
		LUA->Push( 2 );
		LUA->RawGet( -2 );
		return 1;
	}

	LUA_FUNCTION( newindex )
	{
		CheckType( LUA, 1 );

		LUA->GetFEnv( 1 );
		LUA->Push( 2 );
		LUA->Push( 3 );
		LUA->RawSet( -3 );
		return 0;
	}

	LUA_FUNCTION_STATIC( eq )
	{
		const auto left = GetUserData( LUA, 1, false );
		const auto right = GetUserData( LUA, 2, false );
		if( left == nullptr || right == nullptr )
			return false;

		LUA->PushBool( left == right );
		return 1;
	}

	LUA_FUNCTION_STATIC( tostring )
	{
		LUA->PushFormattedString( tostring_format, metaname, Get( LUA, 1 ) );
		return 1;
	}

	LUA_FUNCTION( GetTable )
	{
		CheckType( LUA, 1 );
		LUA->GetFEnv( 1 );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetChannelID )
	{
		const auto channel = Get( LUA, 1 );
		LUA->PushNumber( channel->m_ID );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetName )
	{
		const auto channel = Get( LUA, 1 );
		LUA->PushString( channel->m_Name );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetTags )
	{
		const auto channel = Get( LUA, 1 );

		LUA->CreateTable( );

		uint32_t k = 0;
		for( auto tag = channel->m_pFirstTag; tag != nullptr; tag = tag->m_pNextTag )
		{
			LUA->PushNumber( ++k );

			LUA->PushString( tag->m_pTagName );

			LUA->SetTable( -3 );
		}

		return 1;
	}

	LUA_FUNCTION_STATIC( IsSeverityEnabled )
	{
		const auto channel = Get( LUA, 1 );
		const auto severity = GetSeverityFromLua( LUA, 2 );
		LUA->PushBool( LoggingSystem_IsChannelEnabled( channel->m_ID, severity ) );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetMinimumSeverity )
	{
		const auto channel = Get( LUA, 1 );
		LUA->PushNumber( channel->m_MinimumSeverity );
		return 1;
	}

	LUA_FUNCTION_STATIC( SetMinimumSeverity )
	{
		const auto channel = Get( LUA, 1 );
		const auto severity = GetSeverityFromLua( LUA, 2 );
		LoggingSystem_SetChannelSpewLevel( channel->m_ID, severity );
		return 0;
	}

	LUA_FUNCTION_STATIC( GetColor )
	{
		const auto channel = Get( LUA, 1 );
		ColorToLua( LUA, channel->m_SpewColor );
		return 1;
	}

	LUA_FUNCTION_STATIC( SetColor )
	{
		const auto channel = Get( LUA, 1 );
		LuaToColor( LUA, 2, const_cast<CLoggingSystem::LoggingChannel_t *>( channel )->m_SpewColor );
		return 0;
	}

	LUA_FUNCTION_STATIC( GetFlags )
	{
		LUA->PushNumber( LoggingSystem_GetChannelFlags( Get( LUA, 1 )->m_ID ) );
		return 1;
	}

	LUA_FUNCTION_STATIC( SetFlags )
	{
		const auto channel = Get( LUA, 1 );
		const auto flags = GetNumberFromLua<LoggingChannelFlags_t>( LUA, 2 );
		LoggingSystem_SetChannelFlags( channel->m_ID, flags );
		return 0;
	}

#pragma endregion MetaTable

#pragma region logging library

	LUA_FUNCTION_STATIC( GetChannelCount )
	{
		LUA->PushNumber( LoggingSystem_GetChannelCount( ) );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetChannel )
	{
		const auto channelID = GetNumberFromLua<LoggingChannelID_t>( LUA, 1 );
		if( channelID < 0 || channelID >= LoggingSystem_GetChannelCount( ) )
			return 0;

		Push( LUA, channelID );
		return 1;
	}

	LUA_FUNCTION_STATIC( GetChannels )
	{
		LUA->CreateTable( );

		uint32_t k = 0;
		for(
			LoggingChannelID_t channelID = LoggingSystem_GetFirstChannelID( );
			channelID != INVALID_LOGGING_CHANNEL_ID;
			channelID = LoggingSystem_GetNextChannelID( channelID )
			)
		{
			LUA->PushNumber( ++k );

			Push( LUA, channelID );

			LUA->SetTable( -3 );
		}

		return 1;
	}

	LUA_FUNCTION_STATIC( FindChannel )
	{
		const char *name = LUA->CheckString( 1 );
		const auto channelID = LoggingSystem_FindChannel( name );
		if( channelID == INVALID_LOGGING_CHANNEL_ID )
			return 0;

		Push( LUA, channelID );
		return 1;
	}

	LUA_FUNCTION_STATIC( SetChannelMinimumSeverityByName )
	{
		const char *name = LUA->CheckString( 1 );
		const auto severity = GetSeverityFromLua( LUA, 2 );
		LoggingSystem_SetChannelSpewLevelByName( name, severity );
		return 0;
	}

	LUA_FUNCTION_STATIC( SetChannelMinimumSeverityByTag )
	{
		const char *tag = LUA->CheckString( 1 );
		const auto severity = GetSeverityFromLua( LUA, 2 );
		LoggingSystem_SetChannelSpewLevelByTag( tag, severity );
		return 0;
	}

	LUA_FUNCTION_STATIC( SetGlobalMinimumSeverity )
	{
		const auto severity = GetSeverityFromLua( LUA, 1 );
		LoggingSystem_SetGlobalSpewLevel( severity );
		return 0;
	}

#pragma endregion logging library

	void Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{

#pragma region logging library

		LUA->PushNumber( LS_MESSAGE );
		LUA->SetField( -2, "SEVERITY_MESSAGE" );

		LUA->PushNumber( LS_WARNING );
		LUA->SetField( -2, "SEVERITY_WARNING" );

		LUA->PushNumber( LS_ASSERT );
		LUA->SetField( -2, "SEVERITY_ASSERT" );

		LUA->PushNumber( LS_ERROR );
		LUA->SetField( -2, "SEVERITY_ERROR" );

		LUA->PushNumber( LCF_CONSOLE_ONLY );
		LUA->SetField( -2, "FLAGS_CONSOLE_ONLY" );

		LUA->PushNumber( LCF_DO_NOT_ECHO );
		LUA->SetField( -2, "FLAGS_DO_NOT_ECHO" );

		LUA->PushCFunction( GetChannelCount );
		LUA->SetField( -2, "GetChannelCount" );

		LUA->PushCFunction( GetChannel );
		LUA->SetField( -2, "GetChannel" );

		LUA->PushCFunction( GetChannels );
		LUA->SetField( -2, "GetChannels" );

		LUA->PushCFunction( FindChannel );
		LUA->SetField( -2, "FindChannel" );

		LUA->PushCFunction( SetChannelMinimumSeverityByName );
		LUA->SetField( -2, "SetChannelMinimumSeverityByName" );

		LUA->PushCFunction( SetChannelMinimumSeverityByTag );
		LUA->SetField( -2, "SetChannelMinimumSeverityByTag" );

		LUA->PushCFunction( SetGlobalMinimumSeverity );
		LUA->SetField( -2, "SetGlobalMinimumSeverity" );

#pragma endregion logging library

#pragma region MetaTable

		metatype = LUA->CreateMetaTable( metaname );

		LUA->PushCFunction( eq );
		LUA->SetField( -2, "__eq" );

		LUA->PushCFunction( tostring );
		LUA->SetField( -2, "__tostring" );

		LUA->PushCFunction( index );
		LUA->SetField( -2, "__index" );

		LUA->PushCFunction( newindex );
		LUA->SetField( -2, "__newindex" );

		LUA->PushCFunction( GetTable );
		LUA->SetField( -2, "GetTable" );

		LUA->PushCFunction( GetChannelID );
		LUA->SetField( -2, "GetChannelID" );

		LUA->PushCFunction( GetName );
		LUA->SetField( -2, "GetName" );

		LUA->PushCFunction( GetTags );
		LUA->SetField( -2, "GetTags" );

		LUA->PushCFunction( IsSeverityEnabled );
		LUA->SetField( -2, "IsSeverityEnabled" );

		LUA->PushCFunction( GetMinimumSeverity );
		LUA->SetField( -2, "GetMinimumSeverity" );

		LUA->PushCFunction( SetMinimumSeverity );
		LUA->SetField( -2, "SetMinimumSeverity" );

		LUA->PushCFunction( GetColor );
		LUA->SetField( -2, "GetColor" );

		LUA->PushCFunction( SetColor );
		LUA->SetField( -2, "SetColor" );

		LUA->PushCFunction( GetFlags );
		LUA->SetField( -2, "GetFlags" );

		LUA->PushCFunction( SetFlags );
		LUA->SetField( -2, "SetFlags" );

		LUA->Pop( 1 );

#pragma endregion MetaTable

	}

	void Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{ }
}
