#include "..\..\include\Scripting\LuaStateHolder.h"

lua_State * LuaStateHolder::getLuaState()
{
	if (luaState != nullptr)
	{
		return luaState;
	}
	luaState = luaL_newstate();
	return luaState;
}

void LuaStateHolder::destroyState()
{
	if (luaState != nullptr)
	{
		lua_close(luaState);
	}

}

lua_State * LuaStateHolder::luaState = nullptr;