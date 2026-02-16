// ============================================
// Plant Legends - Entity Implementation
// ============================================

#include "core/entity.hpp"

namespace PL {

u32 Entity::nextId = 1;

Entity::Entity(EntityType type)
    : id(nextId++)
    , type(type)
{
}

void Entity::update(f32 dt) {
    if (!active) return;
    
    updateStatuses(dt);
}

void Entity::addStatus(const StatusEffect& effect) {
    statuses.push_back(effect);
}

void Entity::updateStatuses(f32 dt) {
    for (auto& status : statuses) {
        if (status.active()) {
            status.timer += dt;
        }
    }
    
    // 移除已結束的效果
    statuses.erase(
        std::remove_if(statuses.begin(), statuses.end(),
            [](const StatusEffect& s) { return !s.active(); }),
        statuses.end()
    );
}

bool Entity::hasStatus(StatusType type) const {
    for (const auto& status : statuses) {
        if (status.type == type && status.active()) {
            return true;
        }
    }
    return false;
}

} // namespace PL
