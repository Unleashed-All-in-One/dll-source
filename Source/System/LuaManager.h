#pragma once
extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}
namespace SUC::System
{
	class LuaManager
	{
	public:
		static lua_State* s_LuaState;
		static void Initialize();
		static void OnStageLoad();
		static void OnStageEnd();
	};
}