// ============================================
// Plant Legends - UI 系統
// ============================================

#pragma once

#include "core/types.hpp"
#include "game/game.hpp"
#include "sf3.hpp"
#include <vector>
#include <string>
#include <functional>

namespace PL {

// UI 元素基類
class UIElement {
public:
    UIElement(const SF3::Rect& bounds);
    virtual ~UIElement() = default;
    
    virtual void update(f32 dt);
    virtual void render() = 0;
    
    bool contains(const Vec2& point) const;
    const SF3::Rect& getBounds() const { return bounds; }
    bool isHovered() const { return hovered; }
    bool isClicked() const { return clicked; }
    
protected:
    SF3::Rect bounds;
    bool hovered = false;
    bool clicked = false;
};

// 按鈕
class Button : public UIElement {
public:
    Button(const SF3::Rect& bounds, const std::string& text);
    
    void render() override;
    
    void setOnClick(std::function<void()> callback) { onClick = callback; }
    
private:
    std::string text;
    std::function<void()> onClick;
};

// 植物卡片
class PlantCard : public UIElement {
public:
    PlantCard(const SF3::Rect& bounds, const std::string& plantId);
    
    void update(f32 dt) override;
    void render() override;
    
    const std::string& getPlantId() const { return plantId; }
    bool canAfford(i32 sun) const { return cost <= sun; }
    
private:
    std::string plantId;
    std::string name;
    i32 cost = 0;
    Rarity rarity = Rarity::Common;
    f32 cooldown = 0.0f;
    f32 maxCooldown = 0.0f;
};

// UI 管理器
class UIManager {
public:
    UIManager();
    ~UIManager() = default;
    
    bool initialize();
    void shutdown();
    
    void update(f32 dt, const Game& game);
    void render(const Game& game);
    
    // 事件處理
    void onMouseMove(const Vec2& pos);
    void onMouseClick(const Vec2& pos);
    
    // 植物選擇
    const std::string& getSelectedPlant() const { return selectedPlant; }
    void selectPlant(const std::string& plantId);
    void deselectPlant() { selectedPlant.clear(); }
    
private:
    std::vector<PlantCard> plantCards;
    std::string selectedPlant;
    
    void renderSunDisplay(const Game& game);
    void renderWaveInfo(const Game& game);
    void renderPlantCards(const Game& game);
};

} // namespace PL
