//https://daley-paley.medium.com/super-simple-example-of-adding-lua-to-c-710730e9528a
lua_State* LuaManager::luaState;
static int MyCppFunction(lua_State* L) // Lua callable functions must be this format
{
    const int num = (int)lua_tonumber(L, 1); // get first param from stack
    const char* str = lua_tostring(L, 2); // get second param from stack
    std::cout << "Hello from C++!" << std::endl;
    std::cout << "num = " << num << ", str = " << str << std::endl;
    return 0; // how many params we're passing to Lua
}
static int loadModule(lua_State* L) // Lua callable functions must be this format
{
    const int num = (int)lua_tonumber(L, 1); // get first param from stack
    SequenceHelpers::changeModule((ModuleFlow)num);
    return 0; // how many params we're passing to Lua
}
static int triggerStageLoad(lua_State* L)
{
    const char* stageID = lua_tostring(L, 1);
    int playerID = (int)lua_tonumber(L, 2);
    int stageType = (int)lua_tonumber(L, 3);
    LevelLoadingManager::setGameParameters(stageID, "");
    SequenceHelpers::setPlayerType(playerID);
    LevelLoadingManager::triggerSequenceEvents(stageType);
    return 0; // how many params we're passing to Lua
}
static int triggerMovieLoad(lua_State* L)
{
    const char* evsID = lua_tostring(L, 1);
    const char* stageID = lua_tostring(L, 2);
    int stageType = (int)lua_tonumber(L, 3);
    LevelLoadingManager::setGameParameters(stageID, evsID);
    SequenceHelpers::setPlayerType(0);
    LevelLoadingManager::triggerSequenceEvents(4 + stageType);
    return 0; // how many params we're passing to Lua
}
static int setDefaultStage(lua_State* L)
{
    const char* stageID = lua_tostring(L, 1);
    WRITE_MEMORY(0x015C8200, const char*, stageID);
    return 0;
}
static int hasStageBeenPlayedBefore(lua_State* L)
{
    const char* stageID = lua_tostring(L, 1);
    auto saveObj = SaveManager::getCurrentSave();
    bool index = saveObj->hasStageBeenPlayedBeforeFromID(stageID);
    lua_pushboolean (L, index);
    return 1;
}
static int getSaveBoolKey(lua_State* L)
{
    const char* keyName = lua_tostring(L, 1);
    auto saveObj = SaveManager::getCurrentSave();
    lua_pushboolean(L, saveObj->getSaveBoolKeyValue(keyName));
    return 1;
}
static int getCurrentEVSID(lua_State* L)
{
    lua_pushstring(L, LevelLoadingManager::nextEvsID.c_str());
    return 1;
}
static int getCurrentStageID(lua_State* L)
{
    lua_pushstring(L, LevelLoadingManager::nextStageID.c_str());
    return 1;
}
static int isLoadingFromETF(lua_State* L)
{
    lua_pushboolean(L, LevelLoadingManager::enteredStageFromETF);
    return 1;
}
static int printToScreen(lua_State* L)
{
    const char* printMessage = lua_tostring(L, 1);
    DebugDrawText::log(printMessage, 10);
    return 0;
}
void LuaManager::onStageLoad()
{
    lua_getglobal(luaState, "OnStageLoad"); // find the Lua function
    lua_pcall(luaState, 0, 0, 0); 
}

void LuaManager::initialize()
{
    luaState = luaL_newstate(); // create a new lua instance
    luaL_openlibs(luaState); // give lua access to basic libraries
    lua_register(luaState, "loadModule", loadModule); // register our C++ function with Lua
    lua_register(luaState, "triggerStageLoad", triggerStageLoad); // register our C++ function with Lua
    lua_register(luaState, "triggerMovieLoad", triggerMovieLoad); // register our C++ function with Lua
    lua_register(luaState, "setDefaultStage", setDefaultStage); // register our C++ function with Lua
    lua_register(luaState, "hasStageBeenPlayedBefore", hasStageBeenPlayedBefore); // register our C++ function with Lua
    lua_register(luaState, "getSaveBoolKey", getSaveBoolKey); // register our C++ function with Lua
    lua_register(luaState, "getCurrentEVSID", getCurrentEVSID); // register our C++ function with Lua
    lua_register(luaState, "getCurrentStageID", getCurrentStageID); // register our C++ function with Lua
    lua_register(luaState, "isLoadingFromETF", isLoadingFromETF); // register our C++ function with Lua
    lua_register(luaState, "printToScreen", printToScreen); // register our C++ function with Lua
    luaL_dofile(luaState, "main.lua"); // loads the Lua script
    
    // *** call Lua function from C++ ***
    lua_getglobal(luaState, "OnModInitialize"); // find the Lua function
    lua_pcall(luaState, 0, 0, 0); // call the function passing 2 params
}