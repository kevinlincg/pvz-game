// ============================================
// Plant Legends - Plant 類
// ============================================

#pragma once

#include "core/entity.hpp"
#include <string>

namespace PL {

class Plant : public Entity {
public:
    Plant(const std::string& plantId);
    ~Plant() override = default;
    
    void update(f32 dt) override;
    void render() override;
    
    // 屬性
    const std::string& getPlantId() const { return plantId; }
    const Stats& getStats() const { return stats; }
    
    // 網格位置
    GridCoord getGridPosition() const { return gridPos; }
    void setGridPosition(const GridCoord& pos) { gridPos = pos; }
    
    // 稀有度
    Rarity getRarity() const { return rarity; }
    
    // 元素
    Element getElement() const { return element; }
    
    // 攻擊
    void setTarget(EnemyPtr target) { this->target = target; }
    EnemyPtr getTarget() const { return target; }
    bool canAttack() const;
    void attack();
    
    // 受傷
    void takeDamage(f32 damage);
    
    // 成本
    i32 getCost() const { return cost; }
    
    // 進化
    const std::string& getEvolvesTo() const { return evolvesTo; }
    
private:
    std::string plantId;
    std::string name;
    Stats stats;
    GridCoord gridPos;
    Rarity rarity = Rarity::Common;
    Element element = Element::None;
    
    i32 cost = 100;
    std::string evolvesTo;
    
    f32 attackTimer = 0.0f;
    EnemyPtr target = nullptr;
    
    void loadFromLua();
};

} // namespace PL
