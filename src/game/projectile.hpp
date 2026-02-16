// ============================================
// Plant Legends - Projectile 類
// ============================================

#pragma once

#include "core/entity.hpp"

namespace PL {

class Projectile : public Entity {
public:
    Projectile(const Vec2& startPos, EnemyPtr target, f32 damage);
    ~Projectile() override = default;
    
    void update(f32 dt) override;
    void render() override;
    
    f32 getDamage() const { return damage; }
    EnemyPtr getTarget() const { return target; }
    
private:
    EnemyPtr target;
    f32 damage;
    f32 speed = 500.0f;
    
    void checkHit();
};

} // namespace PL
