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
    const char* num = lua_tostring(L, 1);
    int num = (int)lua_tonumber(L, 2);
    LevelLoadingManager::ActiveReplacement = true;
    LevelLoadingManager::NextLevelLoad = num;
    return 0; // how many params we're passing to Lua
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
    luaL_dofile(luaState, "main.lua"); // loads the Lua script
    
    // *** call Lua function from C++ ***
    lua_getglobal(luaState, "MyLuaFunction"); // find the Lua function
    lua_pushnumber(luaState, 73); // push number as first param
    lua_pushstring(luaState, "From C++ to Lua"); // push string as second param
    lua_pcall(luaState, 2, 0, 0); // call the function passing 2 params
}