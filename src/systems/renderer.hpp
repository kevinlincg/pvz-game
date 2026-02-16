// ============================================
// Plant Legends - 渲染器
// ============================================

#pragma once

#include "core/types.hpp"
#include "game/game.hpp"
#include "sf3.hpp"
#include <memory>

namespace PL {

class Renderer {
public:
    Renderer();
    ~Renderer() = default;
    
    // 初始化
    bool initialize();
    
    // 渲染
    void render(const Game& game);
    
private:
    void renderGrid(const Game& game);
    void renderPlants(const Game& game);
    void renderEnemies(const Game& game);
    void renderProjectiles(const Game& game);
    void renderEffects(const Game& game);
    void renderUI(const Game& game);
    
    // 輔助函數
    SF3::Color getRarityColor(Rarity rarity);
    SF3::Color getHealthColor(f32 hpPercent);
    
    // 動畫
    f32 time = 0.0f;
};

} // namespace PL
