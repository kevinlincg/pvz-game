// ============================================
// Plant Legends - UI System Implementation
// ============================================

#include "ui/ui_system.hpp"
#include "lua/lua_manager.hpp"
#include <iostream>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace PL {

// ============================================
// UIElement
// ============================================

UIElement::UIElement(const SF3::Rect& bounds)
    : bounds(bounds)
{
}

void UIElement::update(f32 dt) {
    // 基礎更新邏輯
}

bool UIElement::contains(const Vec2& point) const {
    return point.x >= bounds.x && point.x <= bounds.x + bounds.w &&
           point.y >= bounds.y && point.y <= bounds.y + bounds.h;
}

// ============================================
// Button
// ============================================

Button::Button(const SF3::Rect& bounds, const std::string& text)
    : UIElement(bounds)
    , text(text)
{
}

void Button::render() {
    using namespace SF3;
    
    Color bgColor = hovered ? Color(80, 80, 80) : Color(60, 60, 60);
    Color borderColor = hovered ? Color(150, 150, 150) : Color(100, 100, 100);
    
    Graphics::drawRect(bounds, bgColor);
    Graphics::drawRectOutline(bounds, borderColor);
    
    // TODO: 添加文字渲染
    // Graphics::drawText(bounds.x + 10, bounds.y + bounds.h/2, text, Color::White(), 16);
}

// ============================================
// PlantCard
// ============================================

PlantCard::PlantCard(const SF3::Rect& bounds, const std::string& plantId)
    : UIElement(bounds)
    , plantId(plantId)
{
    // 從 Lua 載入植物數據
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "plants");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_getfield(L, -1, plantId.c_str());
    if (!lua_istable(L, -1)) {
        lua_pop(L, 2);
        return;
    }
    
    // 讀取名稱
    lua_getfield(L, -1, "name");
    if (lua_isstring(L, -1)) {
        name = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    // 讀取成本
    lua_getfield(L, -1, "cost");
    if (lua_isnumber(L, -1)) {
        cost = (i32)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    
    // 讀取稀有度
    lua_getfield(L, -1, "rarity");
    if (lua_isstring(L, -1)) {
        std::string rarityStr = lua_tostring(L, -1);
        if (rarityStr == "common") rarity = Rarity::Common;
        else if (rarityStr == "rare") rarity = Rarity::Rare;
        else if (rarityStr == "epic") rarity = Rarity::Epic;
        else if (rarityStr == "legendary") rarity = Rarity::Legendary;
    }
    lua_pop(L, 1);
    
    lua_pop(L, 2);  // pop plant and plants
}

void PlantCard::update(f32 dt) {
    UIElement::update(dt);
    
    // 更新冷卻
    if (cooldown > 0) {
        cooldown -= dt;
        if (cooldown < 0) cooldown = 0;
    }
}

void PlantCard::render() {
    using namespace SF3;
    
    // 背景顏色根據稀有度
    Color bgColor;
    switch (rarity) {
        case Rarity::Common:
            bgColor = Color(60, 60, 60);
            break;
        case Rarity::Rare:
            bgColor = Color(40, 60, 100);
            break;
        case Rarity::Epic:
            bgColor = Color(80, 40, 100);
            break;
        case Rarity::Legendary:
            bgColor = Color(100, 80, 20);
            break;
    }
    
    // 如果不能負擔，變暗
    // TODO: 從 Game 獲取陽光數量
    
    // 如果正在冷卻，疊加半透明黑色
    if (cooldown > 0) {
        bgColor.r *= 0.5f;
        bgColor.g *= 0.5f;
        bgColor.b *= 0.5f;
    }
    
    // 如果被懸停，變亮
    if (hovered) {
        bgColor.r = std::min(255, bgColor.r + 30);
        bgColor.g = std::min(255, bgColor.g + 30);
        bgColor.b = std::min(255, bgColor.b + 30);
    }
    
    // 繪製卡片背景
    Graphics::drawRect(bounds, bgColor);
    
    // 邊框
    Color borderColor = hovered ? Color(200, 200, 200) : Color(100, 100, 100);
    Graphics::drawRectOutline(bounds, borderColor);
    
    // 冷卻遮罩
    if (cooldown > 0 && maxCooldown > 0) {
        f32 cdPercent = cooldown / maxCooldown;
        Rect cdMask(bounds.x, bounds.y + bounds.h * (1 - cdPercent), 
                   bounds.w, bounds.h * cdPercent);
        Graphics::drawRect(cdMask, Color(0, 0, 0, 150));
    }
    
    // TODO: 添加文字渲染
    // Graphics::drawText(bounds.x + 5, bounds.y + 5, name, Color::White(), 12);
    // Graphics::drawText(bounds.x + 5, bounds.y + bounds.h - 20, 
    //                    "Cost: " + std::to_string(cost), Color(255, 255, 0), 14);
}

// ============================================
// UIManager
// ============================================

UIManager::UIManager() {
}

bool UIManager::initialize() {
    std::cout << "[UI] Initializing..." << std::endl;
    
    // 創建植物卡片
    std::vector<std::string> startingPlants = {
        "pea_sprite",
        "sun_bloom",
        "nut_guard",
        "twin_sprite",
        "blaze_sprite"
    };
    
    f32 cardWidth = 70;
    f32 cardHeight = 90;
    f32 startX = 10;
    f32 startY = 500;
    
    for (size_t i = 0; i < startingPlants.size(); i++) {
        SF3::Rect cardBounds(startX + i * (cardWidth + 5), startY, cardWidth, cardHeight);
        plantCards.emplace_back(cardBounds, startingPlants[i]);
    }
    
    std::cout << "[UI] Created " << plantCards.size() << " plant cards" << std::endl;
    return true;
}

void UIManager::shutdown() {
    plantCards.clear();
}

void UIManager::update(f32 dt, const Game& game) {
    // 更新所有卡片
    for (auto& card : plantCards) {
        card.update(dt);
    }
}

void UIManager::render(const Game& game) {
    renderSunDisplay(game);
    renderWaveInfo(game);
    renderPlantCards(game);
}

void UIManager::onMouseMove(const Vec2& pos) {
    // 更新卡片懸停狀態
    for (auto& card : plantCards) {
        // TODO: 設置 hovered 狀態
    }
}

void UIManager::onMouseClick(const Vec2& pos) {
    // 檢查卡片點擊
    for (auto& card : plantCards) {
        if (card.contains(pos)) {
            selectPlant(card.getPlantId());
            std::cout << "[UI] Selected plant: " << card.getPlantId() << std::endl;
            return;
        }
    }
    
    // 如果點擊其他地方，取消選擇
    deselectPlant();
}

void UIManager::selectPlant(const std::string& plantId) {
    selectedPlant = plantId;
}

void UIManager::renderSunDisplay(const Game& game) {
    using namespace SF3;
    
    // 陽光背景
    Rect sunBg(10, 10, 150, 40);
    Graphics::drawRect(sunBg, Color(0, 0, 0, 150));
    
    // 陽光圖標（黃色圓形）
    Graphics::drawCircle(30, 30, 15, Color(255, 200, 50));
    
    // TODO: 添加文字渲染
    // Graphics::drawText(50, 25, std::to_string(game.getSun()), Color(255, 255, 0), 24);
}

void UIManager::renderWaveInfo(const Game& game) {
    using namespace SF3;
    
    // 關卡信息背景
    Rect levelBg(10, 60, 200, 30);
    Graphics::drawRect(levelBg, Color(0, 0, 0, 150));
    
    // 波次進度條
    Rect progressBar(15, 90, 190, 10);
    Graphics::drawRect(progressBar, Color(50, 50, 50));
    
    // TODO: 添加實際的波次進度
    f32 progress = 0.5f;
    Rect progressFill(15, 90, 190 * progress, 10);
    Graphics::drawRect(progressFill, Color(100, 200, 100));
}

void UIManager::renderPlantCards(const Game& game) {
    i32 sun = game.getSun();
    
    for (auto& card : plantCards) {
        // TODO: 設置能否負擔的視覺提示
        card.render();
    }
    
    // 繪製選中的卡片高亮
    if (!selectedPlant.empty()) {
        for (const auto& card : plantCards) {
            if (card.getPlantId() == selectedPlant) {
                SF3::Rect highlight = card.getBounds();
                highlight.x -= 2;
                highlight.y -= 2;
                highlight.w += 4;
                highlight.h += 4;
                SF3::Graphics::drawRectOutline(highlight, SF3::Color(255, 255, 0));
            }
        }
    }
}

} // namespace PL
