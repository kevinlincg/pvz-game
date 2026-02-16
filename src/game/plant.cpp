// ============================================
// Plant Legends - Plant Implementation
// ============================================

#include "game/plant.hpp"
#include "game/enemy.hpp"
#include "lua/lua_manager.hpp"
#include <iostream>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace PL {

Plant::Plant(const std::string& plantId)
    : Entity(EntityType::Plant)
    , plantId(plantId)
{
    loadFromLua();
    
    // 設置碰撞盒
    bounds = Rect(position.x - 30, position.y - 30, 60, 60);
}

void Plant::loadFromLua() {
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "plants");
    if (!lua_istable(L, -1)) {
        std::cerr << "[Plant] plants table not found" << std::endl;
        lua_pop(L, 1);
        return;
    }
    
    lua_getfield(L, -1, plantId.c_str());
    if (!lua_istable(L, -1)) {
        std::cerr << "[Plant] " << plantId << " not found" << std::endl;
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
    
    // 讀取元素
    lua_getfield(L, -1, "element");
    if (lua_isstring(L, -1)) {
        std::string elementStr = lua_tostring(L, -1);
        if (elementStr == "fire") element = Element::Fire;
        else if (elementStr == "ice") element = Element::Ice;
        else if (elementStr == "lightning") element = Element::Lightning;
        else if (elementStr == "poison") element = Element::Poison;
    }
    lua_pop(L, 1);
    
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
        
        lua_getfield(L, -1, "attack_speed");
        if (lua_isnumber(L, -1)) {
            stats.attackSpeed = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "range");
        if (lua_isnumber(L, -1)) {
            stats.range = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "crit_rate");
        if (lua_isnumber(L, -1)) {
            stats.critRate = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, -1, "crit_mult");
        if (lua_isnumber(L, -1)) {
            stats.critMult = (f32)lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);  // pop stats
    
    // 讀取進化
    lua_getfield(L, -1, "evolution");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "evolves_to");
        if (lua_isstring(L, -1)) {
            evolvesTo = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);  // pop evolution
    
    lua_pop(L, 2);  // pop plant and plants
    
    std::cout << "[Plant] Loaded " << plantId << " (HP: " << stats.hp 
              << ", DMG: " << stats.damage << ", Cost: " << cost << ")" << std::endl;
}

void Plant::update(f32 dt) {
    Entity::update(dt);
    
    if (!active || !alive) return;
    
    // 更新攻擊計時器
    if (attackTimer > 0) {
        attackTimer -= dt;
    }
    
    // 更新碰撞盒位置
    bounds.x = position.x - 30;
    bounds.y = position.y - 30;
}

void Plant::render() {
    // 渲染將由渲染系統處理
    // 這裡只是佔位符
}

bool Plant::canAttack() const {
    return attackTimer <= 0 && alive && active && target && target->isAlive();
}

void Plant::attack() {
    if (!canAttack()) return;
    
    // 重置攻擊計時器
    attackTimer = 1.0f / stats.attackSpeed;
    
    // 實際攻擊邏輯由戰鬥系統處理
}

void Plant::takeDamage(f32 damage) {
    // 護甲減傷
    f32 actualDamage = damage * (1.0f - stats.armor / 100.0f);
    stats.hp -= actualDamage;
    
    if (stats.hp <= 0) {
        stats.hp = 0;
        alive = false;
        std::cout << "[Plant] " << plantId << " destroyed!" << std::endl;
    }
}

} // namespace PL
