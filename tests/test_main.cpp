// ============================================
// Plant Legends - Tests Main
// ============================================

#include "lua/lua_manager.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>

using namespace PL;

#define TEST(name) std::cout << "[TEST] " << name << "..." << std::endl
#define PASS() std::cout << "  ✓ PASS" << std::endl
#define FAIL(msg) throw std::runtime_error(msg)

// 測試計數器
int tests_passed = 0;
int tests_failed = 0;

void test_config() {
    TEST("Config - Load config.lua");
    
    LuaManager& lua = LuaManager::instance();
    
    if (!lua.loadScript("scripts/config.lua")) {
        FAIL("Failed to load config.lua: " + lua.getLastError());
    }
    
    // config.lua 返回 config table，需要設為全局
    lua_State* L = lua.getState();
    lua_setglobal(L, "config");  // 將返回值設為全局 config
    lua_getglobal(L, "config");
    
    if (!lua_istable(L, -1)) {
        FAIL("config is not a table");
    }
    
    // 測試 game.name
    lua_getfield(L, -1, "game");
    lua_getfield(L, -1, "name");
    std::string game_name = lua_tostring(L, -1);
    lua_pop(L, 2);
    
    if (game_name != "植物戰紀") {
        FAIL("game.name should be '植物戰紀'");
    }
    
    // 測試 global.grid_cols
    lua_getfield(L, -1, "global");
    lua_getfield(L, -1, "grid_cols");
    int grid_cols = (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    
    if (grid_cols != 9) {
        FAIL("global.grid_cols should be 9");
    }
    
    lua_pop(L, 1); // pop config
    
    PASS();
    tests_passed++;
}

void test_plants() {
    TEST("Plants - Load all_plants.lua");
    
    LuaManager& lua = LuaManager::instance();
    
    if (!lua.loadScript("scripts/plants/all_plants.lua")) {
        FAIL("Failed to load all_plants.lua: " + lua.getLastError());
    }
    
    lua_State* L = lua.getState();
    lua_getglobal(L, "plants");
    
    if (!lua_istable(L, -1)) {
        FAIL("plants is not a table");
    }
    
    // 測試豌豆精靈存在
    lua_getfield(L, -1, "pea_sprite");
    if (!lua_istable(L, -1)) {
        FAIL("pea_sprite not found");
    }
    
    // 測試植物名稱
    lua_getfield(L, -1, "name");
    std::string name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    if (name != "豌豆精靈") {
        FAIL("pea_sprite.name should be '豌豆精靈'");
    }
    
    // 測試 stats.damage
    lua_getfield(L, -1, "stats");
    lua_getfield(L, -1, "damage");
    int damage = (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    
    if (damage != 25) {
        FAIL("pea_sprite.stats.damage should be 25");
    }
    
    // 測試稀有度
    lua_getfield(L, -1, "rarity");
    std::string rarity = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    if (rarity != "common") {
        FAIL("pea_sprite.rarity should be 'common'");
    }
    
    lua_pop(L, 1); // pop pea_sprite
    lua_pop(L, 1); // pop plants
    
    PASS();
    tests_passed++;
}

void test_enemies() {
    TEST("Enemies - Load all_enemies.lua");
    
    LuaManager& lua = LuaManager::instance();
    
    if (!lua.loadScript("scripts/enemies/all_enemies.lua")) {
        FAIL("Failed to load all_enemies.lua: " + lua.getLastError());
    }
    
    lua_State* L = lua.getState();
    lua_getglobal(L, "enemies");
    
    if (!lua_istable(L, -1)) {
        FAIL("enemies is not a table");
    }
    
    // 測試腐化史萊姆存在
    lua_getfield(L, -1, "corrupted_slime");
    if (!lua_istable(L, -1)) {
        FAIL("corrupted_slime not found");
    }
    
    // 測試敵人名稱
    lua_getfield(L, -1, "name");
    std::string name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    if (name != "腐化史萊姆") {
        FAIL("corrupted_slime.name should be '腐化史萊姆'");
    }
    
    // 測試 stats.hp
    lua_getfield(L, -1, "stats");
    lua_getfield(L, -1, "hp");
    int hp = (int)lua_tointeger(L, -1);
    lua_pop(L, 2);
    
    if (hp != 50) {
        FAIL("corrupted_slime.stats.hp should be 50");
    }
    
    lua_pop(L, 1); // pop corrupted_slime
    lua_pop(L, 1); // pop enemies
    
    PASS();
    tests_passed++;
}

void test_levels() {
    TEST("Levels - Load all_levels.lua");
    
    LuaManager& lua = LuaManager::instance();
    
    if (!lua.loadScript("scripts/levels/all_levels.lua")) {
        FAIL("Failed to load all_levels.lua: " + lua.getLastError());
    }
    
    lua_State* L = lua.getState();
    lua_getglobal(L, "levels");
    
    if (!lua_istable(L, -1)) {
        FAIL("levels is not a table");
    }
    
    // 測試 1-1 關卡存在
    lua_getfield(L, -1, "1-1");
    if (!lua_istable(L, -1)) {
        FAIL("level 1-1 not found");
    }
    
    // 測試關卡名稱
    lua_getfield(L, -1, "name");
    std::string name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    if (name != "萌芽平原 1-1") {
        FAIL("level 1-1 name should be '萌芽平原 1-1'");
    }
    
    // 測試 waves 存在
    lua_getfield(L, -1, "waves");
    if (!lua_istable(L, -1)) {
        FAIL("level 1-1 waves not found");
    }
    
    // 測試至少有一個 wave
    int wave_count = 0;
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        wave_count++;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    if (wave_count < 1) {
        FAIL("level 1-1 should have at least 1 wave");
    }
    
    lua_pop(L, 1); // pop level 1-1
    lua_pop(L, 1); // pop levels
    
    PASS();
    tests_passed++;
}

void test_evolution() {
    TEST("Evolution - Test plant evolution chain");
    
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "plants");
    
    // 測試豌豆精靈進化鏈
    lua_getfield(L, -1, "pea_sprite");
    lua_getfield(L, -1, "evolution");
    
    if (!lua_istable(L, -1)) {
        FAIL("pea_sprite.evolution not found");
    }
    
    // 測試 requires
    lua_getfield(L, -1, "requires");
    if (!lua_istable(L, -1)) {
        FAIL("pea_sprite.evolution.requires not found");
    }
    
    // 測試需要 3 個
    int requires_count = (int)lua_rawlen(L, -1);
    lua_pop(L, 1);
    
    if (requires_count != 3) {
        FAIL("pea_sprite.evolution.requires should have 3 items");
    }
    
    // 測試 evolves_to
    lua_getfield(L, -1, "evolves_to");
    std::string evolves_to = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    if (evolves_to != "twin_sprite") {
        FAIL("pea_sprite.evolution.evolves_to should be 'twin_sprite'");
    }
    
    lua_pop(L, 1); // pop evolution
    lua_pop(L, 1); // pop pea_sprite
    lua_pop(L, 1); // pop plants
    
    PASS();
    tests_passed++;
}

void test_elements() {
    TEST("Elements - Test element system");
    
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "config");
    lua_getfield(L, -1, "elements");
    
    if (!lua_istable(L, -1)) {
        FAIL("config.elements not found");
    }
    
    // 測試火元素
    lua_getfield(L, -1, "fire");
    if (!lua_istable(L, -1)) {
        FAIL("elements.fire not found");
    }
    
    lua_getfield(L, -1, "burn_damage");
    float burn_damage = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    if (std::abs(burn_damage - 0.1f) > 0.01f) {
        FAIL("fire.burn_damage should be 0.1");
    }
    
    lua_pop(L, 1); // pop fire
    
    // 測試冰元素
    lua_getfield(L, -1, "ice");
    if (!lua_istable(L, -1)) {
        FAIL("elements.ice not found");
    }
    lua_pop(L, 1);
    
    lua_pop(L, 1); // pop elements
    lua_pop(L, 1); // pop config
    
    PASS();
    tests_passed++;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Plant Legends - Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    LuaManager& lua = LuaManager::instance();
    if (!lua.initialize()) {
        std::cerr << "[Error] Failed to initialize Lua" << std::endl;
        return 1;
    }
    
    // 運行測試
    try {
        test_config();
        test_plants();
        test_enemies();
        test_levels();
        test_evolution();
        test_elements();
    } catch (const std::exception& e) {
        std::cerr << "\n[EXCEPTION] " << e.what() << std::endl;
        tests_failed++;
    }
    
    lua.shutdown();
    
    // 結果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Passed: " << tests_passed << std::endl;
    std::cout << "  Failed: " << tests_failed << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return tests_failed > 0 ? 1 : 0;
}
