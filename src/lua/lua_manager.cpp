// ============================================
// Plant Legends - Lua Manager Implementation
// ============================================

#include "lua_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace PL {

LuaManager& LuaManager::instance() {
    static LuaManager inst;
    return inst;
}

bool LuaManager::initialize() {
    L = luaL_newstate();
    if (!L) {
        lastError = "Failed to create Lua state";
        return false;
    }
    
    // 載入標準庫
    luaL_openlibs(L);
    
    std::cout << "[Lua] Initialized successfully" << std::endl;
    return true;
}

void LuaManager::shutdown() {
    if (L) {
        lua_close(L);
        L = nullptr;
        std::cout << "[Lua] Shutdown" << std::endl;
    }
}

bool LuaManager::loadScript(const std::string& path) {
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        handleError();
        return false;
    }
    std::cout << "[Lua] Loaded script: " << path << std::endl;
    return true;
}

bool LuaManager::executeString(const std::string& code) {
    if (luaL_dostring(L, code.c_str()) != LUA_OK) {
        handleError();
        return false;
    }
    return true;
}

void LuaManager::registerFunction(const std::string& name, lua_CFunction func) {
    lua_register(L, name.c_str(), func);
    std::cout << "[Lua] Registered function: " << name << std::endl;
}

bool LuaManager::loadConfig(const std::string& path) {
    if (!loadScript(path)) {
        return false;
    }
    
    // 配置檔案應該返回一個 table
    lua_getglobal(L, "config");
    if (!lua_istable(L, -1)) {
        lastError = "Config file must return a table";
        lua_pop(L, 1);
        return false;
    }
    
    // 將 config table 設為全局變數
    lua_setglobal(L, "config");
    std::cout << "[Lua] Loaded config from: " << path << std::endl;
    return true;
}

bool LuaManager::loadPlants(const std::string& path) {
    if (!loadScript(path)) {
        return false;
    }
    
    // 取得返回的 plants table
    lua_getglobal(L, "plants");
    if (!lua_istable(L, -1)) {
        // 如果 plants 不存在，嘗試從 return value 取得
        if (!lua_istable(L, -1)) {
            lastError = "Plants file must return a table or have a global 'plants' table";
            lua_pop(L, 1);
            return false;
        }
        lua_setglobal(L, "plants");
    } else {
        lua_pop(L, 1);
    }
    
    std::cout << "[Lua] Loaded plants from: " << path << std::endl;
    return true;
}

bool LuaManager::loadEnemies(const std::string& path) {
    if (!loadScript(path)) {
        return false;
    }
    
    lua_getglobal(L, "enemies");
    if (!lua_istable(L, -1)) {
        if (!lua_istable(L, -1)) {
            lastError = "Enemies file must return a table or have a global 'enemies' table";
            lua_pop(L, 1);
            return false;
        }
        lua_setglobal(L, "enemies");
    } else {
        lua_pop(L, 1);
    }
    
    std::cout << "[Lua] Loaded enemies from: " << path << std::endl;
    return true;
}

bool LuaManager::loadLevels(const std::string& path) {
    if (!loadScript(path)) {
        return false;
    }
    
    lua_getglobal(L, "levels");
    if (!lua_istable(L, -1)) {
        if (!lua_istable(L, -1)) {
            lastError = "Levels file must return a table or have a global 'levels' table";
            lua_pop(L, 1);
            return false;
        }
        lua_setglobal(L, "levels");
    } else {
        lua_pop(L, 1);
    }
    
    std::cout << "[Lua] Loaded levels from: " << path << std::endl;
    return true;
}

void LuaManager::handleError() {
    if (lua_gettop(L) > 0 && lua_isstring(L, -1)) {
        lastError = lua_tostring(L, -1);
        std::cerr << "[Lua Error] " << lastError << std::endl;
        lua_pop(L, 1);
    }
}

void LuaManager::dumpStack() {
    int top = lua_gettop(L);
    std::cout << "[Lua Stack] Top: " << top << std::endl;
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        std::cout << "  [" << i << "] " << lua_typename(L, t);
        switch (t) {
            case LUA_TSTRING:
                std::cout << " = " << lua_tostring(L, i);
                break;
            case LUA_TBOOLEAN:
                std::cout << " = " << (lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                std::cout << " = " << lua_tonumber(L, i);
                break;
        }
        std::cout << std::endl;
    }
}

} // namespace PL
