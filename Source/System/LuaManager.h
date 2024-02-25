#pragma once
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
class LuaManager 
{
public:
	static lua_State* luaState;
	static void initialize();
	static void onStageLoad();
};