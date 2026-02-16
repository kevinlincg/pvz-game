#pragma once

#include <scene/sf3_scene.hpp>
#include "pvz_systems.hpp"
#include "pvz_hud.hpp"
#include "pvz_data.hpp"
#include <memory>
#include <vector>

namespace PVZ {

// Save/Load helpers
void saveProgress();
void loadProgress();

// ============================================================================
// Menu Scene
// ============================================================================

class MenuScene : public SF3::Scene {
public:
    MenuScene();
    void onCreate() override;
    void update(SF3::f32 dt) override;
    void render() override;

private:
    float m_titleBob = 0.0f;
    int m_hoverButton = -1;

    struct MenuButton {
        float x, y, w, h;
        const char* label;
    };
    std::vector<MenuButton> m_buttons;

    // Walking zombies on menu background
    struct MenuZombie {
        float x, speed;
        ZombieType type;
        int row;
    };
    std::vector<MenuZombie> m_menuZombies;
};

// ============================================================================
// Level Select Scene
// ============================================================================

class LevelSelectScene : public SF3::Scene {
public:
    LevelSelectScene();
    void onCreate() override;
    void onEnter() override;
    void update(SF3::f32 dt) override;
    void render() override;

    static int unlockedLevel;
    static int selectedLevel;
};

// ============================================================================
// Shop Scene
// ============================================================================

class ShopScene : public SF3::Scene {
public:
    ShopScene();
    void onCreate() override;
    void onEnter() override;
    void update(SF3::f32 dt) override;
    void render() override;

    static int coins;
    static bool plantOwned[PLANT_COUNT];
};

// ============================================================================
// Game Scene
// ============================================================================

class GameScene : public SF3::Scene {
public:
    GameScene();
    void onCreate() override;
    void onEnter() override;
    void onExit() override;
    void update(SF3::f32 dt) override;
    void render() override;

private:
    GameState m_gs;
    HUD m_hud;
    std::vector<LevelDef> m_levels;
    float m_gameOverTimer = 0.0f;
    bool m_transitioned = false;

    // Wave announcement
    float m_waveAnnounceTimer = 0.0f;
    int m_announcedWave = -1;

    // Cloud positions (visual)
    float m_cloudOffset = 0.0f;

    // Camera shake
    float m_shakeTimer = 0.0f;
    float m_shakeIntensity = 0.0f;
    float m_shakeOffsetX = 0.0f;
    float m_shakeOffsetY = 0.0f;

    // Particle effects
    struct Particle {
        float x, y, vx, vy;
        float life, maxLife;
        uint8_t r, g, b;
        float size;
    };
    std::vector<Particle> m_particles;

    void initLevel(int levelIndex);
    void handleGridInput();
    void handleSunClick(float mx, float my);
    void handleDropClick(float mx, float my);
    void renderSky();
    void renderGrid();
    void renderEntities();
    void renderDrops();
    void renderHUD();
    void renderWaveAnnouncement();
    void renderPauseOverlay();
    void renderNightOverlay();
    void renderFogOverlay();
    void renderComboDisplay();
    void renderPowerUpIndicators();
    void renderGameOver();
    void spawnExplosionParticles(float cx, float cy, int count, uint8_t r, uint8_t g, uint8_t b);
    void spawnDeathParticles(float cx, float cy);
    void spawnSplatParticles(float cx, float cy, ProjectileType type);
    void spawnCollectSparkle(float cx, float cy, uint8_t r, uint8_t g, uint8_t b);
    void updateParticles(float dt);
    void renderParticles();
    void renderProjectileTrails();
    void triggerShake(float intensity, float duration);
};

// ============================================================================
// Game Over Scene
// ============================================================================

class GameOverScene : public SF3::Scene {
public:
    GameOverScene();
    void onCreate() override;
    void update(SF3::f32 dt) override;
    void render() override;

    static bool wasVictory;
    static int earnedCoins;
    static int zombiesKilled;
    static int maxCombo;
};

} // namespace PVZ
