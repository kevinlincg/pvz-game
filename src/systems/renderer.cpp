// ============================================
// Plant Legends - Renderer Implementation
// ============================================

#include "systems/renderer.hpp"
#include "game/projectile.hpp"
#include <iostream>

namespace PL {

Renderer::Renderer() {
}

bool Renderer::initialize() {
    std::cout << "[Renderer] Initialized" << std::endl;
    return true;
}

void Renderer::render(const Game& game) {
    time += 0.016f;  // ~60 FPS
    
    renderGrid(game);
    renderPlants(game);
    renderEnemies(game);
    renderProjectiles(game);
    renderEffects(game);
    renderUI(game);
}

void Renderer::renderGrid(const Game& game) {
    using namespace SF3;
    
    for (i32 row = 0; row < 5; row++) {
        for (i32 col = 0; col < 9; col++) {
            Vec2 pos = game.gridToWorld(GridCoord(col, row));
            
            // 交替顏色
            Color cellColor = ((row + col) % 2 == 0) ? Color(60, 130, 90) : Color(55, 125, 85);
            
            // 繪製格子
            Graphics::drawRect(Rect(pos.x - 40, pos.y - 50, 80, 100), cellColor);
            
            // 格子邊框
            Graphics::drawRectOutline(Rect(pos.x - 40, pos.y - 50, 80, 100), Color(40, 100, 70));
        }
    }
}

void Renderer::renderPlants(const Game& game) {
    using namespace SF3;
    
    const auto& plants = game.getPlants();
    
    for (const auto& plant : plants) {
        if (!plant->isAlive()) continue;
        
        Vec2 pos = plant->getPosition();
        const auto& stats = plant->getStats();
        
        // 計算動畫偏移（呼吸效果）
        f32 bounce = std::sin(time * 2.0f + pos.x * 0.02f) * 1.0f;
        
        // 植物主體顏色根據稀有度
        Color plantColor = getRarityColor(plant->getRarity());
        
        // 植物主體
        Rect plantRect(pos.x - 25, pos.y - 25 + bounce, 50, 50);
        Graphics::drawRect(plantRect, plantColor);
        
        // 植物邊框
        Color borderColor = Color(
            plantColor.r * 0.7f,
            plantColor.g * 0.7f,
            plantColor.b * 0.7f
        );
        Graphics::drawRectOutline(plantRect, borderColor);
        
        // 元素效果
        switch (plant->getElement()) {
            case Element::Fire:
                // 火焰光暈
                Graphics::drawCircle(pos.x, pos.y, 30, Color(255, 100, 50, 100));
                break;
            case Element::Ice:
                // 冰霜光暈
                Graphics::drawCircle(pos.x, pos.y, 30, Color(100, 200, 255, 100));
                break;
            case Element::Lightning:
                // 雷電光暈
                Graphics::drawCircle(pos.x, pos.y, 30, Color(255, 255, 100, 100));
                break;
            case Element::Poison:
                // 毒素光暈
                Graphics::drawCircle(pos.x, pos.y, 30, Color(100, 255, 100, 100));
                break;
            default:
                break;
        }
        
        // 血條背景
        Rect hpBarBg(pos.x - 25, pos.y + 30, 50, 4);
        Graphics::drawRect(hpBarBg, Color(100, 100, 100));
        
        // 血條
        f32 hpPercent = stats.hp / stats.maxHp;
        Rect hpBar(pos.x - 25, pos.y + 30, 50 * hpPercent, 4);
        Graphics::drawRect(hpBar, getHealthColor(hpPercent));
    }
}

void Renderer::renderEnemies(const Game& game) {
    using namespace SF3;
    
    const auto& enemies = game.getEnemies();
    
    for (const auto& enemy : enemies) {
        if (!enemy->isAlive()) continue;
        
        Vec2 pos = enemy->getPosition();
        const auto& stats = enemy->getStats();
        
        // 計算動畫偏移（呼吸效果）
        f32 bounce = std::sin(time * 3.0f + pos.x * 0.01f) * 2.0f;
        
        // 敵人主體
        Rect enemyRect(pos.x - 20, pos.y - 20 + bounce, 40, 40);
        
        // 根據行為類型選擇顏色
        Color enemyColor;
        switch (enemy->getBehavior()) {
            case Enemy::Behavior::Flyer:
                enemyColor = Color(150, 100, 200);  // 紫色
                break;
            case Enemy::Behavior::Phasing:
                enemyColor = Color(100, 150, 200);  // 藍色（半透明感）
                break;
            case Enemy::Behavior::Charger:
                enemyColor = Color(200, 100, 50);   // 橙色
                break;
            default:
                enemyColor = Color(200, 50, 50);    // 紅色
        }
        
        Graphics::drawRect(enemyRect, enemyColor);
        
        // 敵人邊框
        Graphics::drawRectOutline(enemyRect, Color(50, 20, 20));
        
        // 血條背景
        Rect hpBarBg(pos.x - 20, pos.y - 30, 40, 5);
        Graphics::drawRect(hpBarBg, Color(100, 100, 100));
        
        // 血條
        f32 hpPercent = stats.hp / stats.maxHp;
        Rect hpBar(pos.x - 20, pos.y - 30, 40 * hpPercent, 5);
        Graphics::drawRect(hpBar, getHealthColor(hpPercent));
        
        // 狀態效果指示器
        if (enemy->isSlowed()) {
            // 藍色光環
            Graphics::drawCircle(pos.x, pos.y, 25, Color(100, 150, 255, 100));
        }
        
        if (enemy->isFrozen()) {
            // 白色冰凍效果
            Graphics::drawRect(Rect(pos.x - 22, pos.y - 22, 44, 44), Color(200, 220, 255, 150));
        }
    }
}

void Renderer::renderProjectiles(const Game& game) {
    using namespace SF3;
    
    const auto& projectiles = game.getProjectiles();
    
    for (const auto& proj : projectiles) {
        if (!proj->isAlive()) continue;
        
        Vec2 pos = proj->getPosition();
        auto target = proj->getTarget();
        
        // 計算投射物方向
        f32 angle = 0.0f;
        if (target && target->isAlive()) {
            Vec2 dir = (target->getPosition() - pos).normalized();
            angle = std::atan2(dir.y, dir.x);
        }
        
        // 繪製投射物（橢圓形）
        Rect projRect(pos.x - 8, pos.y - 4, 16, 8);
        Graphics::drawRect(projRect, Color(100, 255, 100));  // 綠色豌豆
        
        // 尾跡效果
        for (int i = 1; i <= 3; i++) {
            f32 alpha = 255 - (i * 60);
            Rect trail(pos.x - 8 - i * 5, pos.y - 3, 10, 6);
            Graphics::drawRect(trail, Color(100, 255, 100, alpha));
        }
    }
}

void Renderer::renderEffects(const Game& game) {
    using namespace SF3;
    
    // TODO: 添加粒子效果系統
}

void Renderer::renderUI(const Game& game) {
    using namespace SF3;
    
    // 陽光顯示
    Rect sunBg(10, 10, 150, 40);
    Graphics::drawRect(sunBg, Color(0, 0, 0, 150));
    
    // TODO: 添加文字渲染 "Sun: xxx"
    // Graphics::drawText(20, 25, "Sun: " + std::to_string(game.getSun()), Color(255, 255, 0), 20);
    
    // 關卡信息
    Rect levelBg(10, 60, 200, 30);
    Graphics::drawRect(levelBg, Color(0, 0, 0, 150));
    
    // 波次進度
    f32 progress = 0.5f;  // TODO: 從 game 獲取
    Rect progressBar(15, 90, 190, 10);
    Rect progressFill(15, 90, 190 * progress, 10);
    Graphics::drawRect(progressBar, Color(50, 50, 50));
    Graphics::drawRect(progressFill, Color(100, 200, 100));
}

SF3::Color Renderer::getRarityColor(Rarity rarity) {
    using namespace SF3;
    
    switch (rarity) {
        case Rarity::Common:
            return Color(200, 200, 200);    // 灰色
        case Rarity::Rare:
            return Color(100, 150, 255);    // 藍色
        case Rarity::Epic:
            return Color(180, 100, 255);    // 紫色
        case Rarity::Legendary:
            return Color(255, 200, 50);     // 金色
        default:
            return Color(200, 200, 200);
    }
}

SF3::Color Renderer::getHealthColor(f32 hpPercent) {
    using namespace SF3;
    
    if (hpPercent > 0.7f) {
        return Color(50, 255, 50);   // 綠色
    } else if (hpPercent > 0.3f) {
        return Color(255, 200, 50);  // 黃色
    } else {
        return Color(255, 50, 50);   // 紅色
    }
}

} // namespace PL
