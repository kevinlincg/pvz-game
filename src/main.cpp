// ============================================
// Plant Legends - Main Entry Point
// ============================================

#include "sf3.hpp"
#include "lua/lua_manager.hpp"
#include "game/game.hpp"
#include <iostream>

// 使用 SF3 的類型
using SF3::App;
using SF3::Config;
using SF3::Graphics;
using SF3::Input;
using SF3::Key;
using SF3::failed;

using namespace PL;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  植物戰紀 (Plant Legends)" << std::endl;
    std::cout << "  Version 0.1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 初始化 SF3 引擎
    auto& app = App::instance();
    Config config;
    config.title = "植物戰紀 | Plant Legends";
    config.width = 1280;
    config.height = 720;
    config.vsync = true;
    
    if (failed(app.init(config))) {
        std::cerr << "[Error] Failed to initialize SF3 engine" << std::endl;
        return 1;
    }
    
    // 初始化 Lua
    LuaManager& lua = LuaManager::instance();
    if (!lua.initialize()) {
        std::cerr << "[Error] Failed to initialize Lua: " << lua.getLastError() << std::endl;
        return 1;
    }
    
    // 載入遊戲資料
    std::cout << "\n[Loading] Game data..." << std::endl;
    
    if (!lua.loadScript("scripts/config.lua")) {
        std::cerr << "[Error] Failed to load config" << std::endl;
        return 1;
    }
    
    if (!lua.loadScript("scripts/plants/all_plants.lua")) {
        std::cerr << "[Error] Failed to load plants" << std::endl;
        return 1;
    }
    
    if (!lua.loadScript("scripts/enemies/all_enemies.lua")) {
        std::cerr << "[Error] Failed to load enemies" << std::endl;
        return 1;
    }
    
    if (!lua.loadScript("scripts/levels/all_levels.lua")) {
        std::cerr << "[Error] Failed to load levels" << std::endl;
        return 1;
    }
    
    std::cout << "[Success] All game data loaded!\n" << std::endl;
    
    // 初始化遊戲
    Game& game = getGame();
    if (!game.initialize()) {
        std::cerr << "[Error] Failed to initialize game" << std::endl;
        return 1;
    }
    
    // 載入並開始第一關
    if (game.loadLevel("1-1")) {
        game.startLevel();
    }
    
    // 放置一些測試植物
    std::cout << "\n[Game] Placing test plants..." << std::endl;
    game.placePlant("pea_sprite", GridCoord(0, 2));
    game.placePlant("pea_sprite", GridCoord(1, 1));
    game.placePlant("pea_sprite", GridCoord(2, 3));
    game.placePlant("sun_bloom", GridCoord(0, 0));
    game.placePlant("nut_guard", GridCoord(3, 2));
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Game Loop Started" << std::endl;
    std::cout << "  Press ESC to quit" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 主遊戲循環
    while (app.running()) {
        app.pollEvents();
        float dt = app.deltaTime();
        
        // 更新遊戲
        game.update(dt);
        
        // 開始渲染
        Graphics::beginFrame();
        Graphics::clear(SF3::Color(50, 120, 80));  // 綠色背景
        
        // 繪製網格
        for (i32 row = 0; row < 5; row++) {
            for (i32 col = 0; col < 9; col++) {
                PL::Vec2 pos = game.gridToWorld(GridCoord(col, row));
                SF3::Color cellColor = ((row + col) % 2 == 0) ? SF3::Color(60, 130, 90) : SF3::Color(55, 125, 85);
                Graphics::drawRect(SF3::Rect(pos.x - 40, pos.y - 50, 80, 100), cellColor);
            }
        }
        
        // 繪製敵人
        for (const auto& enemy : game.getEnemies()) {
            PL::Vec2 pos = enemy->getPosition();
            Graphics::drawRect(SF3::Rect(pos.x - 20, pos.y - 20, 40, 40), SF3::Color(200, 50, 50));
            
            // 血條
            const auto& stats = enemy->getStats();
            f32 hpPercent = stats.hp / stats.maxHp;
            Graphics::drawRect(SF3::Rect(pos.x - 20, pos.y - 30, 40, 5), SF3::Color(100, 100, 100));
            Graphics::drawRect(SF3::Rect(pos.x - 20, pos.y - 30, 40 * hpPercent, 5), SF3::Color(255, 0, 0));
        }
        
        // 繪製 UI - 陽光
        Graphics::drawRect(SF3::Rect(10, 10, 150, 40), SF3::Color(0, 0, 0, 150));
        // TODO: 繪製文字（Sun: xxx）
        
        // 檢查退出
        if (Input::keyPressed(Key::Escape)) {
            break;
        }
        
        Graphics::endFrame();
    }
    
    // 清理
    game.shutdown();
    lua.shutdown();
    
    std::cout << "\n[Shutdown] Goodbye!" << std::endl;
    return 0;
}
