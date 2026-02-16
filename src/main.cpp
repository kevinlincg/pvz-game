// ============================================
// Plant Legends - Main Entry Point
// ============================================

#include "sf3.hpp"
#include "lua/lua_manager.hpp"
#include "game/game.hpp"
#include "systems/renderer.hpp"
#include "ui/ui_system.hpp"
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
    std::cout << "  Build: " << __DATE__ << " " << __TIME__ << std::endl;
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
    
    // 初始化渲染器
    Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "[Error] Failed to initialize renderer" << std::endl;
        return 1;
    }
    
    // 初始化 UI 系統
    UIManager uiManager;
    if (!uiManager.initialize()) {
        std::cerr << "[Error] Failed to initialize UI" << std::endl;
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
        uiManager.update(dt, game);
        
        // 處理輸入
        // TODO: 鼠標位置和點擊事件
        
        // 開始渲染
        Graphics::beginFrame();
        Graphics::clear(SF3::Color(50, 120, 80));  // 綠色背景
        
        // 使用渲染器
        renderer.render(game);
        
        // 渲染 UI（在最上層）
        uiManager.render(game);
        
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
