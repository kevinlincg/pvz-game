// ============================================
// Plant Legends - Projectile Implementation
// ============================================

#include "game/projectile.hpp"
#include "game/enemy.hpp"
#include <iostream>

namespace PL {

Projectile::Projectile(const Vec2& startPos, EnemyPtr target, f32 damage)
    : Entity(EntityType::Projectile)
    , target(target)
    , damage(damage)
{
    setPosition(startPos);
    bounds = SF3::Rect(startPos.x - 5, startPos.y - 5, 10, 10);
}

void Projectile::update(f32 dt) {
    if (!alive || !target || !target->isAlive()) {
        destroy();
        return;
    }
    
    // 朝目標移動
    Vec2 direction = (target->getPosition() - position).normalized();
    position = position + direction * speed * dt;
    
    // 更新碰撞盒
    bounds.x = position.x - 5;
    bounds.y = position.y - 5;
    
    // 檢查是否命中
    checkHit();
}

void Projectile::render() {
    // 渲染將由渲染系統處理
}

void Projectile::checkHit() {
    if (!target || !target->isAlive()) return;
    
    // 檢查碰撞
    if (bounds.intersects(target->getBounds())) {
        // 命中！
        target->takeDamage(damage);
        
        std::cout << "[Projectile] Hit enemy for " << damage << " damage!" << std::endl;
        
        // 檢查爆擊
        // TODO: 從植物獲取爆擊率
        
        destroy();
    }
}

} // namespace PL
