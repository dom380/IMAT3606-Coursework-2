#pragma once
extern "C"
{
#include <lua\lua.h>
#include <lua\lauxlib.h>
#include <lua\lualib.h>
}
class LuaStateHolder {
public:
	static lua_State* getLuaState();
	static void destroyState();
private:
	static lua_State* luaState;
};
