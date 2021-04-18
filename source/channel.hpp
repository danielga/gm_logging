#pragma once

namespace GarrysMod::Lua
{
	class ILuaBase;
}

namespace logging::channel
{
	void Initialize( GarrysMod::Lua::ILuaBase *LUA );
	void Deinitialize( GarrysMod::Lua::ILuaBase *LUA );
}
