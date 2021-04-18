#pragma once

#include <GarrysMod/Lua/Interface.h>

#include <cstdint>
#include <limits>

class Color;

namespace logging::helpers
{
	void ColorToLua( GarrysMod::Lua::ILuaBase *LUA, const Color &color );
	void LuaToColor( GarrysMod::Lua::ILuaBase *LUA, const int32_t index, Color &color );

	template<typename NumberType>
	static NumberType GetNumberFromLua(
		GarrysMod::Lua::ILuaBase *LUA,
		const int32_t index,
		const char *oob_error = "number is out of bounds",
		const NumberType min = std::numeric_limits<NumberType>::min( ),
		const NumberType max = std::numeric_limits<NumberType>::max( )
	)
	{
		const double number = LUA->CheckNumber( index );
		if( number < min || number > max )
			LUA->ArgError( 1, oob_error );

		return static_cast<NumberType>( number );
	}
}
