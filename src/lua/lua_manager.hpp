// ============================================
// Plant Legends - Lua Manager
// ============================================

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

// Forward declarations
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace PL {

class LuaManager {
public:
    static LuaManager& instance();
    
    // 初始化 Lua 環境
    bool initialize();
    void shutdown();
    
    // 執行腳本
    bool loadScript(const std::string& path);
    bool executeString(const std::string& code);
    
    // 取得 Lua state
    lua_State* getState() { return L; }
    
    // 註冊 C++ 函數給 Lua 調用
    void registerFunction(const std::string& name, lua_CFunction func);
    
    // 從 Lua 取得數值
    template<typename T>
    T getGlobal(const std::string& name);
    
    // 設定 Lua 全局變數
    template<typename T>
    void setGlobal(const std::string& name, T value);
    
    // 讀取配置檔
    bool loadConfig(const std::string& path);
    
    // 讀取植物定義
    bool loadPlants(const std::string& path);
    
    // 讀取敵人定義
    bool loadEnemies(const std::string& path);
    
    // 讀取關卡定義
    bool loadLevels(const std::string& path);
    
    // 呼叫 Lua 函數
    template<typename R, typename... Args>
    R callFunction(const std::string& name, Args... args);
    
    // 錯誤處理
    std::string getLastError() const { return lastError; }
    
private:
    LuaManager() : L(nullptr) {}
    ~LuaManager() { shutdown(); }
    
    lua_State* L;
    std::string lastError;
    
    // 輔助函數
    void handleError();
    void dumpStack();
};

// ============================================
// Template implementations
// ============================================

template<>
inline int LuaManager::getGlobal<int>(const std::string& name) {
    lua_getglobal(L, name.c_str());
    int value = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return value;
}

template<>
inline float LuaManager::getGlobal<float>(const std::string& name) {
    lua_getglobal(L, name.c_str());
    float value = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);
    return value;
}

template<>
inline std::string LuaManager::getGlobal<std::string>(const std::string& name) {
    lua_getglobal(L, name.c_str());
    std::string value = lua_tostring(L, -1);
    lua_pop(L, 1);
    return value;
}

template<>
inline void LuaManager::setGlobal<int>(const std::string& name, int value) {
    lua_pushinteger(L, value);
    lua_setglobal(L, name.c_str());
}

template<>
inline void LuaManager::setGlobal<float>(const std::string& name, float value) {
    lua_pushnumber(L, value);
    lua_setglobal(L, name.c_str());
}

template<>
inline void LuaManager::setGlobal<std::string>(const std::string& name, std::string value) {
    lua_pushstring(L, value.c_str());
    lua_setglobal(L, name.c_str());
}

} // namespace PL
