// ============================================
// Plant Legends - Main Entry Point
// ============================================

#include "sf3.hpp"
#include "lua/lua_manager.hpp"
#include <iostream>

using namespace SF3;
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
    
    // 載入配置
    std::cout << "\n[Loading] Game configuration..." << std::endl;
    if (!lua.loadScript("scripts/config.lua")) {
        std::cerr << "[Error] Failed to load config: " << lua.getLastError() << std::endl;
        return 1;
    }
    
    // 載入植物定義
    std::cout << "[Loading] Plant definitions..." << std::endl;
    if (!lua.loadScript("scripts/plants/all_plants.lua")) {
        std::cerr << "[Error] Failed to load plants: " << lua.getLastError() << std::endl;
        return 1;
    }
    
    // 載入敵人定義
    std::cout << "[Loading] Enemy definitions..." << std::endl;
    if (!lua.loadScript("scripts/enemies/all_enemies.lua")) {
        std::cerr << "[Error] Failed to load enemies: " << lua.getLastError() << std::endl;
        return 1;
    }
    
    // 載入關卡定義
    std::cout << "[Loading] Level definitions..." << std::endl;
    if (!lua.loadScript("scripts/levels/all_levels.lua")) {
        std::cerr << "[Error] Failed to load levels: " << lua.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "\n[Success] All game data loaded!" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 主遊戲循環
    while (app.running()) {
        app.pollEvents();
        float dt = app.deltaTime();
        
        // 開始渲染
        Graphics::beginFrame();
        Graphics::clear(Color{50, 120, 80});
        
        // 繪製測試文字（暫時用簡單矩形代替）
        Graphics::drawRect({40, 40, 600, 50}, Color{255, 255, 255});
        Graphics::drawRect({40, 90, 300, 40}, Color{200, 200, 200});
        Graphics::drawRect({40, 140, 200, 30}, Color{150, 150, 150});
        
        // 檢查退出
        if (Input::keyPressed(Key::Escape)) {
            break;
        }
        
        Graphics::endFrame();
    }
    
    // 清理
    lua.shutdown();
    
    std::cout << "\n[Shutdown] Goodbye!" << std::endl;
    return 0;
}
