// ============================================
// Plant Legends - Enemy Implementation
// ============================================

#include "game/enemy.hpp"
#include "game/plant.hpp"
#include "lua/lua_manager.hpp"
#include <iostream>
#include <random>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace PL {

Enemy::Enemy(const std::string& enemyId)
    : Entity(EntityType::Enemy)
    , enemyId(enemyId)
{
    loadFromLua();
    
    // 設置碰撞盒
    bounds = Rect(position.x - 25, position.y - 25, 50, 50);
}

void Enemy::loadFromLua() {
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "enemies");
    if (!lua_istable(L, -1)) {
        std::cerr << "[Enemy] enemies table not found" << std::endl;
        lua_pop(L, 1);
        return;
    }
    
    lua_getfield(L, -1, enemyId.c_str());
    if (!lua_istable(L, -1)) {
        std::cerr << "[Enemy] " << enemyId << " not found" << std::endl;
        lua_pop(L, 2);
        return;
    }
    
    // 讀取名稱
    lua_getfield(L, -1, "name");
    if (lua_isstring(L, -1)) {
        name = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    // 讀取行為類型
    lua_getfield(L, -1, "behavior");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "type");
        if (lua_isstring(L, -1)) {
            std::string typeStr = lua_tostring(L, -1);
            if (typeStr == "walker") behavior = Behavior::Walker;
            else if (typeStr == "flyer") behavior = Behavior::Flyer;
            else if (typeStr == "phasing") behavior = Behavior::Phasing;
            else if (typeStr == "charger") behavior = Behavior::Charger;
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);  // pop behavior
    
    // 讀取統計數據
    lua_getfield(L, -1, "stats");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "hp");
        if (lua_isnumber(L, -1)) {
            stats.hp = (f32)lua_tonumber(L, -1);
            stats.maxHp = stats.hp;
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "damage");
        if (lua_isnumber(L, -1)) {
            stats.damage = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "speed");
        if (lua_isnumber(L, -1)) {
            stats.speed = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "armor");
        if (lua_isnumber(L, -1)) {
            stats.armor = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);  // pop stats
    
    lua_pop(L, 2);  // pop enemy and enemies
    
    std::cout << "[Enemy] Loaded " << enemyId << " (HP: " << stats.hp 
              << ", DMG: " << stats.damage << ", Speed: " << stats.speed << ")" << std::endl;
}

void Enemy::update(f32 dt) {
    Entity::update(dt);
    
    if (!active || !alive) return;
    
    // 更新攻擊計時器
    if (attackTimer > 0) {
        attackTimer -= dt;
    }
    
    // 如果沒有凍結，則移動
    if (!isFrozen()) {
        move(dt);
    }
    
    // 更新碰撞盒位置
    bounds.x = position.x - 25;
    bounds.y = position.y - 25;
}

void Enemy::render() {
    // 渲染將由渲染系統處理
}

void Enemy::move(f32 dt) {
    // 計算實際速度（考慮減速效果）
    f32 actualSpeed = stats.speed;
    if (isSlowed()) {
        actualSpeed *= 0.5f;  // 減速 50%
    }
    
    // 向左移動
    position.x -= actualSpeed * dt;
    
    // 更新碰撞盒
    bounds.x = position.x - 25;
}

f32 Enemy::getProgress() const {
    // 假設起點 x=1200, 終點 x=0
    f32 start = 1200.0f;
    f32 end = 0.0f;
    return 1.0f - (position.x - end) / (start - end);
}

void Enemy::takeDamage(f32 damage) {
    // 護甲減傷
    f32 actualDamage = damage * (1.0f - stats.armor / 100.0f);
    stats.hp -= actualDamage;
    
    if (stats.hp <= 0) {
        stats.hp = 0;
        alive = false;
        std::cout << "[Enemy] " << enemyId << " defeated!" << std::endl;
    }
}

bool Enemy::canAttack() const {
    return attackTimer <= 0 && alive && active && targetPlant && targetPlant->isAlive();
}

void Enemy::attack() {
    if (!canAttack()) return;
    
    // 攻擊目標植物
    targetPlant->takeDamage(stats.damage);
    
    // 重置攻擊計時器
    attackTimer = 1.0f;  // 1秒攻擊間隔
    
    std::cout << "[Enemy] " << enemyId << " attacks plant for " << stats.damage << " damage!" << std::endl;
}

} // namespace PL
