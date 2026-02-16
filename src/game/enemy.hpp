// ============================================
// Plant Legends - Enemy 類
// ============================================

#pragma once

#include "core/entity.hpp"

namespace PL {

class Enemy : public Entity {
public:
    Enemy(const std::string& enemyId);
    ~Enemy() override = default;
    
    void update(f32 dt) override;
    void render() override;
    
    // 屬性
    const std::string& getEnemyId() const { return enemyId; }
    const Stats& getStats() const { return stats; }
    
    // 位置
    i32 getRow() const { return row; }
    void setRow(i32 r) { row = r; }
    
    // 移動
    void move(f32 dt);
    f32 getProgress() const;  // 0.0 = 起點, 1.0 = 終點
    
    // 戰鬥
    void takeDamage(f32 damage);
    bool canAttack() const;
    void attack();
    
    // 目標
    PlantPtr getTargetPlant() const { return targetPlant; }
    void setTargetPlant(PlantPtr plant) { targetPlant = plant; }
    
    // 行為類型
    enum class Behavior {
        Walker,      // 直線前進
        Flyer,       // 飛行
        Phasing,     // 可穿透植物
        Charger      // 衝鋒
    };
    
    Behavior getBehavior() const { return behavior; }
    
    // 狀態效果
    bool isSlowed() const { return hasStatus(StatusType::Slow); }
    bool isFrozen() const { return hasStatus(StatusType::Freeze); }
    
private:
    std::string enemyId;
    std::string name;
    Stats stats;
    i32 row = 0;
    Behavior behavior = Behavior::Walker;
    
    f32 attackTimer = 0.0f;
    PlantPtr targetPlant = nullptr;
    
    void loadFromLua();
};

} // namespace PL
