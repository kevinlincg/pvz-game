#pragma once

#include "sf3.hpp"
#include "pvz_data.hpp"

namespace PVZ {

// ============================================================================
// Position / Movement
// ============================================================================

struct GridPos {
    int row = -1;
    int col = -1;
};

struct WorldPos {
    SF3::Vec2 pos{0, 0};
};

struct Velocity {
    SF3::Vec2 vel{0, 0};
};

// ============================================================================
// Health / Damage
// ============================================================================

struct Health {
    int current = 100;
    int max = 100;
    float damageFlash = 0.0f;
};

struct Armor {
    int current = 0;
    int max = 0;
};

struct Damage {
    int amount = 20;
};

// ============================================================================
// Plant Components
// ============================================================================

struct Plant {
    PlantType type = PlantType::Peashooter;
};

struct AttackTimer {
    float cooldown = 1.5f;
    float timer = 0.0f;
};

struct SunProducer {
    float interval = 5.0f;
    float timer = 0.0f;
};

struct PotatoMineState {
    float armTimer = 15.0f;
    bool armed = false;
};

struct ChomperState {
    float digestTimer = 0.0f;
    bool eating = false;
};

// ============================================================================
// Zombie Components
// ============================================================================

struct Zombie {
    ZombieType type = ZombieType::Basic;
    bool eating = false;
    float eatTimer = 0.0f;
    float eatRate = 0.5f;
};

struct ZombieSpecial {
    bool hasJumped = false;     // Pole vault
    bool isAngry = false;       // Newspaper
    float summonTimer = 0.0f;   // Dancing
    bool hasSummoned = false;
};

// Boss zombie state (Gargantuar)
struct BossState {
    float smashTimer = 0.0f;      // Timer until next smash attack
    float smashCooldown = 4.0f;   // Seconds between smashes
    bool hasThrown = false;        // Thrown imp at half HP
    int phase = 1;                 // 1 = full HP, 2 = below 50%
};

// ============================================================================
// Projectile
// ============================================================================

struct Projectile {
    ProjectileType type = ProjectileType::Pea;
    float speed = 300.0f;
    int damage = 20;
    int row = 0;
};

struct Freezable {
    float slowFactor = 1.0f;
    float timer = 0.0f;
};

// ============================================================================
// Sun
// ============================================================================

struct Sun {
    float targetY = 0.0f;
    bool falling = true;
    bool collecting = false;
    SF3::Vec2 collectTarget{0, 0};
    float lifetime = 8.0f;
};

// ============================================================================
// Lawnmower
// ============================================================================

struct Lawnmower {
    int row = 0;
    bool triggered = false;
    float speed = 400.0f;
};

// ============================================================================
// Drop / Power-Up (spawned when zombie dies)
// ============================================================================

struct Drop {
    DropType type = DropType::Coin;
    float targetY = 0.0f;
    bool falling = true;
    bool collecting = false;
    SF3::Vec2 collectTarget{0, 0};
    float lifetime = 12.0f;
};

// ============================================================================
// Visual / Animation
// ============================================================================

struct SpriteAnim {
    int frameIndex = 0;
    float timer = 0.0f;
    int frameCount = 2;
    float fps = 4.0f;
};

struct ExplosionEffect {
    float timer = 0.0f;
    float duration = 0.5f;
    float radius = 0.0f;
};

// ============================================================================
// Tags
// ============================================================================

struct TagEnemy {};
struct TagPlant {};
struct TagBullet {};
struct TagSun {};
struct TagLawnmower {};
struct TagEffect {};
struct TagDrop {};

} // namespace PVZ
