#pragma once

#include "ecs/sf3_ecs.hpp"
#include "pvz_components.hpp"
#include "pvz_data.hpp"
#include <functional>

namespace PVZ {

class GameState;

// ============================================================================
// Game State
// ============================================================================

class GameState {
public:
    SF3::World world;

    // Economy
    int sunCount = 50;

    // Level state
    int currentLevel = 0;
    int currentWave = 0;
    float waveTimer = 0.0f;
    float spawnTimer = 0.0f;
    int zombiesSpawned = 0;
    int waveEntryIndex = 0;
    bool levelComplete = false;
    bool levelFailed = false;
    int totalZombiesAlive = 0;
    int totalZombiesKilled = 0;
    int coinsEarned = 0;

    // Grid occupancy
    SF3::Entity grid[GRID_ROWS][GRID_COLS] = {};
    SF3::Entity lawnmowers[GRID_ROWS] = {};

    // Sun drop timer
    float skyDropTimer = 5.0f;

    // HUD state
    int selectedPlant = -1;
    bool shovelActive = false;
    float seedCooldowns[PLANT_COUNT] = {};

    // Level definition
    const LevelDef* levelDef = nullptr;

    // Wave announcements
    bool newWaveStarted = false;

    // Game speed
    float gameSpeed = 1.0f;
    bool paused = false;

    // Combo system
    int comboCount = 0;         // current kill streak
    float comboTimer = 0.0f;    // time since last kill (resets on kill)
    int maxCombo = 0;           // best combo this level
    float comboDisplayTimer = 0.0f; // for displaying combo text

    // Active power-ups
    float plantBoostTimer = 0.0f;   // PlantBoost: 2x attack speed
    float freezeAllTimer = 0.0f;    // FreezeAll: all zombies frozen

    // Entities pending destruction
    std::vector<SF3::Entity> pendingDestroy;

    // Callbacks
    std::function<void(int)> onSunCollected;

    void destroyEntity(SF3::Entity e) {
        pendingDestroy.push_back(e);
    }

    void flushDestroyQueue() {
        for (auto e : pendingDestroy) {
            for (int r = 0; r < GRID_ROWS; r++)
                for (int c = 0; c < GRID_COLS; c++)
                    if (grid[r][c] == e) grid[r][c] = 0;
            for (int r = 0; r < GRID_ROWS; r++)
                if (lawnmowers[r] == e) lawnmowers[r] = 0;
            world.destroy(e);
        }
        pendingDestroy.clear();
    }
};

// ============================================================================
// System Functions
// ============================================================================

// Plant systems
void systemPlantAttack(GameState& gs, float dt);
void systemSunProduction(GameState& gs, float dt);
void systemPotatoMine(GameState& gs, float dt);
void systemChomper(GameState& gs, float dt);
void systemCherryBomb(GameState& gs, float dt);
void systemJalapeno(GameState& gs, float dt);

// Projectile systems
void systemProjectileMove(GameState& gs, float dt);
void systemProjectileCollision(GameState& gs, float dt);
void systemTorchwood(GameState& gs, float dt);

// Zombie systems
void systemZombieMove(GameState& gs, float dt);
void systemZombieEat(GameState& gs, float dt);
void systemZombieSpecial(GameState& gs, float dt);
void systemZombieSpawn(GameState& gs, float dt);
void systemBossAI(GameState& gs, float dt);

// Sun systems
void systemSunFall(GameState& gs, float dt);
void systemSunCollect(GameState& gs, float dt);
void systemSkySunDrop(GameState& gs, float dt);

// Drop system
void systemDrops(GameState& gs, float dt);

// Lawnmower
void systemLawnmower(GameState& gs, float dt);

// Animation
void systemAnimation(GameState& gs, float dt);

// Freeze
void systemFreeze(GameState& gs, float dt);

// Effects
void systemExplosions(GameState& gs, float dt);

// Power-ups
void systemPowerUps(GameState& gs, float dt);

// Win/Lose check
void systemWinLose(GameState& gs, float dt);

// Entity creation helpers
SF3::Entity createPlant(GameState& gs, PlantType type, int row, int col);
SF3::Entity createZombie(GameState& gs, ZombieType type, int row);
SF3::Entity createProjectile(GameState& gs, ProjectileType type, float x, float y, int row, int damage);
SF3::Entity createSun(GameState& gs, float x, float y, float targetY);
SF3::Entity createLawnmower(GameState& gs, int row);
SF3::Entity createDrop(GameState& gs, DropType type, float x, float y);

} // namespace PVZ
