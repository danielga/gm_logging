#include "helpers.hpp"

#include <GarrysMod/Lua/Interface.h>

#include <color.h>

namespace logging::helpers
{
	void ColorToLua( GarrysMod::Lua::ILuaBase *LUA, const Color &color )
	{
		LUA->CreateTable( );

			LUA->PushNumber( color.r( ) );
			LUA->SetField( -2, "r" );

			LUA->PushNumber( color.g( ) );
			LUA->SetField( -2, "g" );

			LUA->PushNumber( color.b( ) );
			LUA->SetField( -2, "b" );

			LUA->PushNumber( color.a( ) );
			LUA->SetField( -2, "a" );
	}

	void LuaToColor( GarrysMod::Lua::ILuaBase *LUA, const int32_t index, Color &color )
	{
		if( !LUA->IsType( index, GarrysMod::Lua::Type::Table ) )
			LUA->ArgError( index, "color needs to be a table" );

		LUA->GetField( index, "r" );
		const auto red = GetNumberFromLua<uint8_t>( LUA, -1, "amount of red is out of bounds" );
		LUA->Pop( 1 );

		LUA->GetField( index, "g" );
		const auto green = GetNumberFromLua<uint8_t>( LUA, -1, "amount of green is out of bounds" );
		LUA->Pop( 1 );

		LUA->GetField( index, "b" );
		const auto blue = GetNumberFromLua<uint8_t>( LUA, -1, "amount of blue is out of bounds" );
		LUA->Pop( 1 );

		LUA->GetField( index, "a" );
		const auto alpha = GetNumberFromLua<uint8_t>( LUA, -1, "amount of alpha is out of bounds" );
		LUA->Pop( 1 );

		color.SetColor( red, green, blue, alpha );
	}
}
