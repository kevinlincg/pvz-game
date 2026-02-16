// ============================================
// Plant Legends - Entity 基類
// ============================================

#pragma once

#include "core/types.hpp"
#include <vector>
#include <memory>

namespace PL {

class Entity {
public:
    Entity(EntityType type);
    virtual ~Entity() = default;
    
    // 生命週期
    virtual void update(f32 dt);
    virtual void render() = 0;
    
    // 狀態
    bool isAlive() const { return alive; }
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    
    // 位置  
    const Vec2& getPosition() const { return position; }
    void setPosition(const Vec2& pos) { position = pos; }
    
    // 類型
    EntityType getType() const { return type; }
    
    // ID
    u32 getId() const { return id; }
    
    // 碰撞
    const Rect& getBounds() const { return bounds; }
    void setBounds(const Rect& b) { bounds = b; }
    
    // 狀態效果
    void addStatus(const StatusEffect& effect);
    void updateStatuses(f32 dt);
    bool hasStatus(StatusType type) const;
    
    // 標記為銷毀
    void destroy() { alive = false; }
    
protected:
    u32 id;
    EntityType type;
    Vec2 position;
    Rect bounds;
    bool alive = true;
    bool active = true;
    
    std::vector<StatusEffect> statuses;
    
    static u32 nextId;
};

} // namespace PL
