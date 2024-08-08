//https://daley-paley.medium.com/super-simple-example-of-adding-lua-to-c-710730e9528a
namespace SUC::System
{
	lua_State* LuaManager::s_LuaState;
	static int MyCppFunction(lua_State* L) // Lua callable functions must be this format
	{
		const int num = (int)lua_tonumber(L, 1); // get first param from stack
		const char* str = lua_tostring(L, 2); // get second param from stack
		std::cout << "Hello from C++!" << std::endl;
		std::cout << "num = " << num << ", str = " << str << std::endl;
		return 0; // how many params we're passing to Lua
	}
	static int loadModule(lua_State* L) 
	{
		const int num = (int)lua_tonumber(L, 1); // get first param from stack
		SequenceHelpers::ChangeModule((ModuleFlow)num);
		return 0; 
	}
	static int triggerStageLoad(lua_State* L)
	{
		const char* stageID = lua_tostring(L, 1);
		int playerID = (int)lua_tonumber(L, 2);
		int stageType = (int)lua_tonumber(L, 3);
		SUC::System::StageManager::SetGameParameters(stageID, "");
		SUC::System::StageManager::TriggerSequenceEvents(stageType, true);
		SequenceHelpers::SetPlayerType(playerID, true);
		return 0; 
	}
	static int triggerMovieLoad(lua_State* L)
	{
		const char* evsID = lua_tostring(L, 1);
		const char* stageID = lua_tostring(L, 2);
		int playerType = (int)lua_tonumber(L, 3);
		int stageType = (int)lua_tonumber(L, 4);
		SUC::System::StageManager::SetGameParameters(stageID, evsID);
		SUC::System::StageManager::TriggerSequenceEvents(4 + stageType, true);
		SequenceHelpers::SetPlayerType(playerType, true);
		return 0;
	}
	static int setDefaultStage(lua_State* L)
	{
		const char* stageID = lua_tostring(L, 1);
		WRITE_MEMORY(0x015C8200, char*, stageID);
		return 0;
	}
	static int hasStageBeenPlayedBefore(lua_State* L)
	{
		const char* stageID = lua_tostring(L, 1);
		auto saveObj = SaveManager::GetCurrentSave();
		bool index = saveObj->HasStageBeenPlayedBeforeFromID(stageID);
		lua_pushboolean(L, index);
		return 1;
	}
	static int getSaveBoolKey(lua_State* L)
	{
		const char* keyName = lua_tostring(L, 1);
		auto saveObj = SaveManager::GetCurrentSave();
		lua_pushboolean(L, saveObj->GetSaveBoolKeyValue(keyName));
		return 1;
	}
	static int getCurrentEVSID(lua_State* L)
	{
		lua_pushstring(L, SUC::System::StageManager::s_NextEventScene.c_str());
		return 1;
	}
	static int getCurrentStageID(lua_State* L)
	{
		lua_pushstring(L, SUC::System::StageManager::s_NextStage.c_str());
		return 1;
	}
	static int isLoadingFromETF(lua_State* L)
	{
		lua_pushboolean(L, SUC::System::StageManager::s_HubModeEnabled);
		return 1;
	}
	static int printToScreen(lua_State* L)
	{
		const char* printMessage = lua_tostring(L, 1);
		DebugDrawText::log(printMessage, 10);
		return 0;
	}
	void LuaManager::OnStageLoad()
	{
		lua_getglobal(s_LuaState, "OnStageLoad"); // find the Lua function
		lua_pcall(s_LuaState, 0, 0, 0);
	}
	void LuaManager::OnStageEnd()
	{
		lua_getglobal(s_LuaState, "OnStageEnd"); // find the Lua function
		lua_pcall(s_LuaState, 0, 0, 0); // call the function passing 2 params
	}
	void LuaManager::Initialize()
	{
		s_LuaState = luaL_newstate(); // create a new lua instance
		luaL_openlibs(s_LuaState); // give lua access to basic libraries
		lua_register(s_LuaState, "loadModule", loadModule); 
		lua_register(s_LuaState, "triggerStageLoad", triggerStageLoad); 
		lua_register(s_LuaState, "triggerMovieLoad", triggerMovieLoad); 
		lua_register(s_LuaState, "setDefaultStage", setDefaultStage); 
		lua_register(s_LuaState, "hasStageBeenPlayedBefore", hasStageBeenPlayedBefore); 
		lua_register(s_LuaState, "getSaveBoolKey", getSaveBoolKey); 
		lua_register(s_LuaState, "getCurrentEVSID", getCurrentEVSID); 
		lua_register(s_LuaState, "getCurrentStageID", getCurrentStageID); 
		lua_register(s_LuaState, "isLoadingFromETF", isLoadingFromETF); 
		lua_register(s_LuaState, "printToScreen", printToScreen); 
		luaL_dofile(s_LuaState, LUA_FILE); // loads the Lua script

		// *** call Lua function from C++ ***
		lua_getglobal(s_LuaState, "OnModInitialize"); // find the Lua function
		lua_pcall(s_LuaState, 0, 0, 0); // call the function passing 2 params
	}
}