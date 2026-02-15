#include "pvz_scenes.hpp"
#include "pvz_renderer.hpp"
#include "pvz_font.hpp"
#include "save/sf3_save.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace PVZ {

// ============================================================================
// Save / Load
// ============================================================================

void saveProgress() {
    auto& saver = SF3::SaveSystem::instance();
    SF3::SaveData data;
    data.set("unlockedLevel", SF3::SaveData(LevelSelectScene::unlockedLevel));
    data.set("coins", SF3::SaveData(ShopScene::coins));

    SF3::SaveData plants;
    for (int i = 0; i < PLANT_COUNT; i++) {
        plants.append(SF3::SaveData(ShopScene::plantOwned[i]));
    }
    data.set("plantOwned", plants);
    saver.save("pvz_progress", data, false, true);
}

void loadProgress() {
    auto& saver = SF3::SaveSystem::instance();
    SF3::SaveData data;
    if (saver.load("pvz_progress", data, false)) {
        LevelSelectScene::unlockedLevel = data["unlockedLevel"].getInt(1);
        ShopScene::coins = data["coins"].getInt(0);
        if (data.has("plantOwned")) {
            auto& plants = data["plantOwned"];
            for (int i = 0; i < PLANT_COUNT && i < static_cast<int>(plants.size()); i++) {
                ShopScene::plantOwned[i] = plants[static_cast<size_t>(i)].getBool(false);
            }
        }
    }
}

// Static members
int LevelSelectScene::unlockedLevel = 1;
int LevelSelectScene::selectedLevel = 1;
int ShopScene::coins = 0;
bool ShopScene::plantOwned[PLANT_COUNT] = {
    true,  // Sunflower (free)
    true,  // Peashooter (free)
    true,  // Wall-nut (free)
    false, false, false, false, false, false, false
};
bool GameOverScene::wasVictory = false;
int GameOverScene::earnedCoins = 0;
int GameOverScene::zombiesKilled = 0;
int GameOverScene::maxCombo = 0;

// ============================================================================
// Menu Scene
// ============================================================================

MenuScene::MenuScene() : Scene("menu") {}

void MenuScene::onCreate() {
    float cx = SCREEN_W * 0.5f;
    m_buttons = {
        {cx - 80, 300, 160, 50, "Play"},
        {cx - 80, 370, 160, 50, "Shop"},
        {cx - 80, 440, 160, 50, "Quit"},
    };
    // Spawn background walking zombies
    ZombieType types[] = {ZombieType::Basic, ZombieType::Cone, ZombieType::Basic,
                          ZombieType::Flag, ZombieType::Bucket};
    for (int i = 0; i < 5; i++) {
        float x = SCREEN_W + 50 + (std::rand() % 300);
        float speed = 15.0f + (std::rand() % 15);
        int row = i % 3;
        m_menuZombies.push_back({x, speed, types[i], row});
    }
}

void MenuScene::update(SF3::f32 dt) {
    m_titleBob += dt;

    // Update walking zombies
    for (auto& z : m_menuZombies) {
        z.x -= z.speed * dt;
        if (z.x < -60) {
            z.x = SCREEN_W + 30 + (std::rand() % 200);
            z.speed = 15.0f + (std::rand() % 15);
        }
    }

    auto mpos = SF3::Input::mousePos();
    m_hoverButton = -1;
    for (int i = 0; i < static_cast<int>(m_buttons.size()); i++) {
        auto& b = m_buttons[i];
        if (mpos.x >= b.x && mpos.x <= b.x + b.w && mpos.y >= b.y && mpos.y <= b.y + b.h) {
            m_hoverButton = i;
        }
    }

    if (SF3::Input::mousePressed(SF3::MouseButton::Left) && m_hoverButton >= 0) {
        switch (m_hoverButton) {
            case 0: switchTo("levelselect", SF3::SceneTransition::Fade(0.3f)); break;
            case 1: switchTo("shop", SF3::SceneTransition::Fade(0.3f)); break;
            case 2: SF3::App::instance().quit(); break;
        }
    }
}

void MenuScene::render() {
    // Sky gradient background
    for (int i = 0; i < 6; i++) {
        float t = static_cast<float>(i) / 5.0f;
        uint8_t r = static_cast<uint8_t>(100 + t * 50);
        uint8_t g = static_cast<uint8_t>(160 + t * 60);
        uint8_t b = static_cast<uint8_t>(220 + t * 30);
        float h = SCREEN_H * 0.4f / 6.0f;
        SF3::Graphics::drawRect({0, i * h, SCREEN_W, h + 1}, SF3::Color(r, g, b));
    }

    // Lawn area
    SF3::Graphics::drawRect({0, SCREEN_H * 0.4f, SCREEN_W, SCREEN_H * 0.6f}, SF3::Color(55, 110, 35));

    // Decorative grass rows
    for (int row = 0; row < 4; row++) {
        float gy = SCREEN_H * 0.4f + row * 40;
        for (int i = 0; i < 20; i++) {
            float gx = i * 50.0f;
            SF3::Color gc = ((i + row) % 2 == 0) ? SF3::Color(60, 120, 40) : SF3::Color(50, 105, 35);
            SF3::Graphics::drawRect({gx, gy, 50, 40}, gc);
        }
    }

    // Dirt path at bottom
    SF3::Graphics::drawRect({0, SCREEN_H - 50, SCREEN_W, 50}, SF3::Color(120, 90, 50));

    // Title
    float bobY = std::sin(m_titleBob * 2.0f) * 8.0f;
    float titleY = 80 + bobY;
    auto& fm = FontManager::instance();

    // Title background plate
    float cx = SCREEN_W * 0.5f;
    SF3::Graphics::drawRect({cx - 170, titleY - 10, 340, 65}, SF3::Color(60, 30, 10, 200));
    SF3::Graphics::drawRectOutline({cx - 170, titleY - 10, 340, 65}, SF3::Color(100, 60, 20));

    fm.drawTextCenteredShadow(u8"植物大戰殭屍", cx, titleY,
                              SF3::Color(80, 200, 50));
    fm.drawSmallCentered(u8"SF3 Engine Edition", cx, titleY + 35,
                         SF3::Color(180, 220, 160));

    // Decorative plants scattered
    Renderer::drawSunflower(80, 250, static_cast<int>(m_titleBob * 3) % 2);
    Renderer::drawSunflower(180, 300, (static_cast<int>(m_titleBob * 3) + 1) % 2);
    Renderer::drawPeashooter(SCREEN_W - 80, 260, static_cast<int>(m_titleBob * 2) % 2);
    Renderer::drawWallNut(SCREEN_W - 160, 310, 1.0f);
    Renderer::drawTorchwood(140, 400, static_cast<int>(m_titleBob * 2) % 2);

    // Buttons
    for (int i = 0; i < static_cast<int>(m_buttons.size()); i++) {
        auto& b = m_buttons[i];
        bool hover = (i == m_hoverButton);
        SF3::Color bg = hover ? SF3::Color(100, 180, 60) : SF3::Color(70, 130, 40);
        SF3::Color border = hover ? SF3::Color(150, 220, 80) : SF3::Color(50, 100, 30);

        // Button shadow
        SF3::Graphics::drawRect({b.x + 3, b.y + 3, b.w, b.h}, SF3::Color(20, 40, 10));
        SF3::Graphics::drawRect({b.x, b.y, b.w, b.h}, bg);
        SF3::Graphics::drawRectOutline({b.x, b.y, b.w, b.h}, border);

        const char* labels[] = {u8"開始遊戲", u8"商店", u8"離開"};
        const char* label = (i < 3) ? labels[i] : b.label;
        fm.drawTextCentered(label, b.x + b.w * 0.5f, b.y + b.h * 0.5f - 12,
                            SF3::Color(255, 255, 230));
    }

    // Walking zombies across the lawn
    for (auto& z : m_menuZombies) {
        float zy = SCREEN_H * 0.45f + z.row * 55;
        int frame = static_cast<int>(m_titleBob * 2 + z.x * 0.02f) % 4;
        // Shadow
        SF3::Graphics::drawCircleFilled(z.x, zy + 30, 12, SF3::Color(0, 0, 0, 20));
        Renderer::drawZombie(z.type, z.x, zy, frame, 1.0f, 1.0f, false);
    }

    // Animated sun bouncing in corner
    float sunBob = std::sin(m_titleBob * 3.0f) * 6.0f;
    Renderer::drawSun(70, 70 + sunBob, 1.2f);
}

// ============================================================================
// Level Select Scene
// ============================================================================

LevelSelectScene::LevelSelectScene() : Scene("levelselect") {}

void LevelSelectScene::onCreate() {}

void LevelSelectScene::onEnter() {}

void LevelSelectScene::update(SF3::f32 dt) {
    (void)dt;
    auto mpos = SF3::Input::mousePos();

    if (SF3::Input::mousePressed(SF3::MouseButton::Left)) {
        // Check level buttons (matching render layout)
        for (int i = 0; i < 10; i++) {
            float bx = 100 + (i % 5) * 140;
            float by = 120 + (i / 5) * 220;
            if (mpos.x >= bx && mpos.x <= bx + 120 && mpos.y >= by && mpos.y <= by + 160) {
                if (i + 1 <= unlockedLevel) {
                    selectedLevel = i + 1;
                    switchTo("game", SF3::SceneTransition::Fade(0.3f));
                }
                return;
            }
        }

        // Back button
        if (mpos.x >= 20 && mpos.x <= 120 && mpos.y >= 20 && mpos.y <= 60) {
            switchTo("menu", SF3::SceneTransition::Fade(0.3f));
        }
    }

    if (SF3::Input::keyPressed(SF3::Key::Escape)) {
        switchTo("menu", SF3::SceneTransition::Fade(0.3f));
    }
}

void LevelSelectScene::render() {
    static auto levelDefs = buildLevelDefs();

    SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(40, 80, 30));
    auto& fm = FontManager::instance();

    // Decorative grass strips
    for (int i = 0; i < 20; i++) {
        float gx = i * 50.0f;
        SF3::Color gc = (i % 2 == 0) ? SF3::Color(50, 95, 35) : SF3::Color(45, 85, 30);
        SF3::Graphics::drawRect({gx, SCREEN_H - 40, 50, 40}, gc);
    }

    // Title bar
    SF3::Graphics::drawRect({0, 0, SCREEN_W, 80}, SF3::Color(50, 30, 10, 230));
    SF3::Graphics::drawRect({0, 75, SCREEN_W, 5}, SF3::Color(80, 50, 20));
    fm.drawTextCenteredShadow(u8"選擇關卡", SCREEN_W * 0.5f, 20, SF3::Color(80, 200, 50));

    // Back button
    SF3::Graphics::drawRect({20, 20, 100, 40}, SF3::Color(100, 60, 30));
    SF3::Graphics::drawRectOutline({20, 20, 100, 40}, SF3::Color(150, 100, 50));
    fm.drawTextCentered(u8"返回", 70, 25, SF3::Color(255, 230, 200));

    // Coin display
    Renderer::drawSunIcon(SCREEN_W - 130, 25, 22);
    char coinBuf[32];
    std::snprintf(coinBuf, sizeof(coinBuf), "%d", ShopScene::coins);
    fm.drawText(coinBuf, SCREEN_W - 100, 20, SF3::Color(255, 220, 50));

    // Level grid
    auto mpos = SF3::Input::mousePos();
    for (int i = 0; i < 10; i++) {
        float bx = 100 + (i % 5) * 140;
        float by = 120 + (i / 5) * 220;
        bool unlocked = (i + 1 <= unlockedLevel);
        bool completed = (i + 1 < unlockedLevel);
        bool hover = unlocked && mpos.x >= bx && mpos.x <= bx + 120 && mpos.y >= by && mpos.y <= by + 160;

        // Card background
        SF3::Color bg = unlocked
            ? (hover ? SF3::Color(85, 150, 50) : SF3::Color(70, 130, 40))
            : SF3::Color(55, 55, 55);
        SF3::Graphics::drawRect({bx, by, 120, 160}, bg);
        SF3::Graphics::drawRectOutline({bx, by, 120, 160},
            hover ? SF3::Color(130, 200, 70) : SF3::Color(40, 80, 25));

        // Level number
        char numBuf[16];
        std::snprintf(numBuf, sizeof(numBuf), "%d", i + 1);
        SF3::Color numC = unlocked ? SF3::Color(255, 255, 200) : SF3::Color(100, 100, 100);
        fm.drawTextCentered(numBuf, bx + 60, by + 10, numC);

        // Level name (from data)
        if (i < static_cast<int>(levelDefs.size())) {
            SF3::Color nameC = unlocked ? SF3::Color(200, 220, 180) : SF3::Color(90, 90, 90);
            fm.drawSmallCentered(levelDefs[i].nameZh, bx + 60, by + 45, nameC);
        }

        // Level type icon and mini decoration
        if (unlocked && i < static_cast<int>(levelDefs.size())) {
            auto& ldef = levelDefs[i];
            // Type indicator in top-right corner of card
            if (ldef.isNight && ldef.hasFog) {
                // Night + Fog: dark purple circle
                SF3::Graphics::drawCircleFilled(bx + 102, by + 18, 8, SF3::Color(60, 40, 90, 200));
                SF3::Graphics::drawCircleFilled(bx + 99, by + 15, 4, SF3::Color(180, 180, 220, 150));
            } else if (ldef.isNight) {
                // Night: moon crescent
                SF3::Graphics::drawCircleFilled(bx + 102, by + 18, 8, SF3::Color(220, 220, 150));
                SF3::Graphics::drawCircleFilled(bx + 105, by + 15, 6, SF3::Color(bg.r, bg.g, bg.b));
            } else if (ldef.hasFog) {
                // Fog: gray clouds
                SF3::Graphics::drawCircleFilled(bx + 100, by + 18, 6, SF3::Color(180, 190, 200, 180));
                SF3::Graphics::drawCircleFilled(bx + 106, by + 19, 5, SF3::Color(190, 200, 210, 160));
            } else {
                // Day: sun
                SF3::Graphics::drawCircleFilled(bx + 102, by + 18, 7, SF3::Color(255, 220, 60));
                SF3::Graphics::drawCircleFilled(bx + 100, by + 16, 3, SF3::Color(255, 240, 120));
            }

            // Mini plant/zombie preview based on level theme
            float anim = SF3::App::instance().elapsedTime();
            int frame = static_cast<int>(anim * 2.0f + i) % 2;
            Renderer::drawPeashooter(bx + 35, by + 90, frame);
            if (i >= 3) {
                ZombieType zt = ZombieType::Basic;
                if (i >= 7) zt = ZombieType::Dancing;
                else if (i >= 5) zt = ZombieType::PoleVault;
                else zt = ZombieType::Cone;
                Renderer::drawZombie(zt, bx + 85, by + 95, frame, 1.0f, 1.0f, false);
            }
        }

        // Stars for completed levels
        if (completed) {
            for (int s = 0; s < 3; s++) {
                float sx = bx + 22 + s * 30;
                float sy = by + 130;
                SF3::Graphics::drawCircleFilled(sx + 8, sy + 8, 8, SF3::Color(255, 220, 50));
                SF3::Graphics::drawCircleFilled(sx + 8, sy + 5, 4, SF3::Color(255, 240, 100));
            }
            fm.drawSmallCentered(u8"通過", bx + 60, by + 145, SF3::Color(180, 255, 140));
        }

        // Lock icon for locked levels
        if (!unlocked) {
            SF3::Graphics::drawRect({bx + 45, by + 75, 30, 25}, SF3::Color(120, 120, 120));
            SF3::Graphics::drawRectOutline({bx + 50, by + 65, 20, 15}, SF3::Color(120, 120, 120));
            fm.drawSmallCentered(u8"鎖定", bx + 60, by + 110, SF3::Color(150, 150, 150));
        }
    }
}

// ============================================================================
// Shop Scene
// ============================================================================

ShopScene::ShopScene() : Scene("shop") {}
void ShopScene::onCreate() {}
void ShopScene::onEnter() {}

void ShopScene::update(SF3::f32 dt) {
    (void)dt;
    auto mpos = SF3::Input::mousePos();

    if (SF3::Input::mousePressed(SF3::MouseButton::Left)) {
        // Plant items
        for (int i = 0; i < PLANT_COUNT; i++) {
            float ix = 80 + (i % 5) * 160;
            float iy = 150 + (i / 5) * 200;

            if (mpos.x >= ix && mpos.x <= ix + 140 && mpos.y >= iy && mpos.y <= iy + 170) {
                if (!plantOwned[i]) {
                    int cost = 50 + i * 25; // shop price
                    if (coins >= cost) {
                        coins -= cost;
                        plantOwned[i] = true;
                    }
                }
                return;
            }
        }

        // Back
        if (mpos.x >= 20 && mpos.x <= 120 && mpos.y >= 20 && mpos.y <= 60) {
            switchTo("menu", SF3::SceneTransition::Fade(0.3f));
        }
    }

    if (SF3::Input::keyPressed(SF3::Key::Escape)) {
        switchTo("menu", SF3::SceneTransition::Fade(0.3f));
    }
}

void ShopScene::render() {
    // Wood plank background
    SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(60, 40, 25));
    for (int i = 0; i < 12; i++) {
        float plankY = i * 50.0f;
        SF3::Color pc = (i % 2 == 0) ? SF3::Color(65, 44, 28) : SF3::Color(58, 38, 22);
        SF3::Graphics::drawRect({0, plankY, SCREEN_W, 50}, pc);
        SF3::Graphics::drawLine(0, plankY, SCREEN_W, plankY, SF3::Color(45, 30, 15), 1);
    }
    auto& fm = FontManager::instance();

    // Header
    SF3::Graphics::drawRect({0, 0, SCREEN_W, 70}, SF3::Color(50, 30, 10, 230));
    SF3::Graphics::drawRect({0, 65, SCREEN_W, 5}, SF3::Color(80, 50, 20));
    fm.drawTextCenteredShadow(u8"商店", SCREEN_W * 0.5f, 18, SF3::Color(255, 220, 100));

    // Coin display
    Renderer::drawSunIcon(SCREEN_W - 130, 20, 24);
    char coinBuf[32];
    std::snprintf(coinBuf, sizeof(coinBuf), "%d", coins);
    fm.drawText(coinBuf, SCREEN_W - 100, 15, SF3::Color(255, 220, 50));

    // Back button
    SF3::Graphics::drawRect({20, 20, 100, 40}, SF3::Color(100, 60, 30));
    SF3::Graphics::drawRectOutline({20, 20, 100, 40}, SF3::Color(150, 100, 50));
    fm.drawTextCentered(u8"返回", 70, 25, SF3::Color(255, 230, 200));

    // Plant items
    for (int i = 0; i < PLANT_COUNT; i++) {
        float ix = 80 + (i % 5) * 160;
        float iy = 150 + (i / 5) * 200;

        SF3::Color bg = plantOwned[i] ? SF3::Color(50, 90, 40) : SF3::Color(80, 60, 40);
        SF3::Graphics::drawRect({ix, iy, 140, 170}, bg);
        SF3::Graphics::drawRectOutline({ix, iy, 140, 170}, SF3::Color(100, 80, 50));

        // Animated plant preview (gentle bob)
        float bob = std::sin(SF3::App::instance().elapsedTime() * 2.0f + i * 0.8f) * 3.0f;
        int frame = static_cast<int>(SF3::App::instance().elapsedTime() * 3.0f + i) % 2;
        Renderer::drawPlant(static_cast<PlantType>(i), ix + 70, iy + 70 + bob, frame);

        // Plant name (from data)
        auto& pdata = getPlantData(static_cast<PlantType>(i));
        fm.drawSmallCentered(pdata.nameZh, ix + 70, iy + 115, SF3::Color(220, 220, 200));

        // Status
        if (plantOwned[i]) {
            SF3::Graphics::drawRect({ix + 30, iy + 140, 80, 20}, SF3::Color(60, 140, 40));
            fm.drawSmallCentered(u8"已擁有", ix + 70, iy + 141, SF3::Color(200, 255, 200));
        } else {
            int cost = 50 + i * 25;
            SF3::Color costBg = (coins >= cost) ? SF3::Color(180, 150, 50) : SF3::Color(100, 80, 60);
            SF3::Graphics::drawRect({ix + 30, iy + 140, 80, 20}, costBg);
            char priceBuf[16];
            std::snprintf(priceBuf, sizeof(priceBuf), "%d", cost);
            fm.drawSmallCentered(priceBuf, ix + 70, iy + 141,
                (coins >= cost) ? SF3::Color(255, 255, 200) : SF3::Color(150, 150, 150));
        }
    }
}

// ============================================================================
// Game Scene
// ============================================================================

GameScene::GameScene() : Scene("game") {}

void GameScene::onCreate() {
    m_levels = buildLevelDefs();
}

void GameScene::onEnter() {
    int lvl = LevelSelectScene::selectedLevel;
    initLevel(lvl - 1);
}

void GameScene::onExit() {
    // Cleanup
}

void GameScene::initLevel(int levelIndex) {
    if (levelIndex < 0 || levelIndex >= static_cast<int>(m_levels.size())) return;

    m_gs = GameState(); // reset
    m_gs.levelDef = &m_levels[levelIndex];
    m_gs.sunCount = m_gs.levelDef->startingSun;
    m_gs.currentLevel = levelIndex;
    m_gameOverTimer = 0;
    m_transitioned = false;
    m_waveAnnounceTimer = 0;
    m_announcedWave = -1;
    m_particles.clear();

    // Build available plants list (only owned ones)
    std::vector<PlantType> available;
    for (auto pt : m_gs.levelDef->availablePlants) {
        if (ShopScene::plantOwned[static_cast<int>(pt)]) {
            available.push_back(pt);
        }
    }

    m_hud.init(&m_gs, available);

    // Create lawnmowers
    for (int r = 0; r < GRID_ROWS; r++) {
        createLawnmower(m_gs, r);
    }
}

void GameScene::handleGridInput() {
    auto mpos = SF3::Input::mousePos();
    if (!SF3::Input::mousePressed(SF3::MouseButton::Left)) return;
    if (mpos.y < HUD::BAR_H) return;

    int row = rowFromY(mpos.y);
    int col = colFromX(mpos.x);
    if (row < 0 || col < 0) return;

    if (m_gs.shovelActive) {
        // Remove plant
        if (m_gs.grid[row][col] != 0) {
            m_gs.destroyEntity(m_gs.grid[row][col]);
            m_gs.grid[row][col] = 0;
            m_hud.clearSelection();
        }
        return;
    }

    if (m_gs.selectedPlant >= 0) {
        auto plantType = m_hud.getSelectedPlantType();
        auto& data = getPlantData(plantType);

        if (m_gs.grid[row][col] == 0 && m_gs.sunCount >= data.cost) {
            createPlant(m_gs, plantType, row, col);
            m_gs.sunCount -= data.cost;
            m_gs.seedCooldowns[static_cast<int>(plantType)] = data.cooldown;
            m_hud.clearSelection();
        }
    }
}

void GameScene::handleSunClick(float mx, float my) {
    if (!SF3::Input::mousePressed(SF3::MouseButton::Left)) return;

    // Check sun entities
    m_gs.world.each<Sun, WorldPos>(
        [&](SF3::Entity e, Sun& sun, WorldPos& wp) {
            if (sun.collecting) return;
            float dist = std::sqrt((mx - wp.pos.x) * (mx - wp.pos.x) + (my - wp.pos.y) * (my - wp.pos.y));
            if (dist < 25.0f) {
                sun.collecting = true;
                sun.collectTarget = {60.0f, 30.0f};
                spawnCollectSparkle(wp.pos.x, wp.pos.y, 255, 230, 80);
            }
        });
}

void GameScene::handleDropClick(float mx, float my) {
    if (!SF3::Input::mousePressed(SF3::MouseButton::Left)) return;

    m_gs.world.each<Drop, WorldPos>(
        [&](SF3::Entity e, Drop& drop, WorldPos& wp) {
            if (drop.collecting) return;
            float dist = std::sqrt((mx - wp.pos.x) * (mx - wp.pos.x) + (my - wp.pos.y) * (my - wp.pos.y));
            if (dist < 30.0f) {
                drop.collecting = true;
                drop.collectTarget = {SCREEN_W * 0.5f, 30.0f};
                // Sparkle on collect
                uint8_t sr = 255, sg = 210, sb = 50;
                switch (drop.type) {
                    case DropType::Diamond:    sr = 100; sg = 180; sb = 255; break;
                    case DropType::PlantBoost: sr = 60; sg = 220; sb = 60; break;
                    case DropType::FreezeAll:  sr = 100; sg = 180; sb = 240; break;
                    case DropType::SunBurst:   sr = 255; sg = 200; sb = 50; break;
                    default: break;
                }
                spawnCollectSparkle(wp.pos.x, wp.pos.y, sr, sg, sb);
                // Effect applied in systemDrops when drop reaches target
            }
        });
}

void GameScene::update(SF3::f32 dt) {
    // Clouds always move
    m_cloudOffset += dt * 15.0f;
    if (m_cloudOffset > SCREEN_W + 200) m_cloudOffset -= SCREEN_W + 400;

    // Pause toggle
    if (SF3::Input::keyPressed(SF3::Key::Escape)) {
        if (m_gs.paused) {
            m_gs.paused = false;
        } else if (!m_gs.levelComplete && !m_gs.levelFailed) {
            m_gs.paused = true;
        } else {
            switchTo("menu", SF3::SceneTransition::Fade(0.3f));
            return;
        }
    }

    // Fast-forward toggle
    if (SF3::Input::keyPressed(SF3::Key::Space) && !m_gs.levelComplete && !m_gs.levelFailed) {
        m_gs.gameSpeed = (m_gs.gameSpeed > 1.5f) ? 1.0f : 2.0f;
    }

    // Pause: only handle sun clicking (so collected suns still animate)
    if (m_gs.paused) {
        // Return to menu from pause
        if (SF3::Input::keyPressed(SF3::Key::Q)) {
            switchTo("menu", SF3::SceneTransition::Fade(0.3f));
        }
        return;
    }

    float gameDt = dt * m_gs.gameSpeed;

    // Wave announcement timer
    if (m_waveAnnounceTimer > 0) {
        m_waveAnnounceTimer -= dt;  // real time, not game time
    }

    if (m_gs.levelComplete || m_gs.levelFailed) {
        m_gameOverTimer += dt;
        if (m_gameOverTimer >= 2.0f && !m_transitioned) {
            m_transitioned = true;
            GameOverScene::wasVictory = m_gs.levelComplete;
            GameOverScene::earnedCoins = m_gs.coinsEarned;
            GameOverScene::zombiesKilled = m_gs.totalZombiesKilled;
            GameOverScene::maxCombo = m_gs.maxCombo;

            if (m_gs.levelComplete) {
                ShopScene::coins += m_gs.coinsEarned;
                if (m_gs.currentLevel + 1 >= LevelSelectScene::unlockedLevel) {
                    LevelSelectScene::unlockedLevel = std::min(10, m_gs.currentLevel + 2);
                }
                saveProgress();
            }

            switchTo("gameover", SF3::SceneTransition::Fade(0.5f));
        }
        updateParticles(dt);
        return;
    }

    m_hud.update(gameDt);

    auto mpos = SF3::Input::mousePos();
    m_hud.handleInput(mpos.x, mpos.y, SF3::Input::mousePressed(SF3::MouseButton::Left));
    handleGridInput();
    handleSunClick(mpos.x, mpos.y);
    handleDropClick(mpos.x, mpos.y);

    // Check for wave announcements
    if (m_gs.newWaveStarted) {
        m_gs.newWaveStarted = false;
        if (m_gs.currentWave < static_cast<int>(m_gs.levelDef->waves.size())) {
            auto& wave = m_gs.levelDef->waves[m_gs.currentWave];
            if (wave.isFlagWave) {
                m_waveAnnounceTimer = 3.0f;
                m_announcedWave = m_gs.currentWave;
            }
        }
    }

    // Track zombie count before systems for death particle spawning
    int prevKilled = m_gs.totalZombiesKilled;

    // Run all systems with game speed
    systemPlantAttack(m_gs, gameDt);
    systemSunProduction(m_gs, gameDt);
    systemPotatoMine(m_gs, gameDt);
    systemChomper(m_gs, gameDt);

    // Cherry Bomb / Jalapeno - trigger screen shake + particles
    m_gs.world.each<Plant, ExplosionEffect, WorldPos>(
        [&](SF3::Entity, Plant& p, ExplosionEffect& fx, WorldPos& wp) {
            if (fx.timer < 0.05f) {
                if (p.type == PlantType::CherryBomb) {
                    spawnExplosionParticles(wp.pos.x, wp.pos.y, 30, 255, 150, 50);
                    triggerShake(8.0f, 0.4f);
                } else if (p.type == PlantType::Jalapeno) {
                    float y = wp.pos.y;
                    for (int c = 0; c < GRID_COLS; c++) {
                        spawnExplosionParticles(cellCenterX(c), y, 5, 255, 100, 20);
                    }
                    triggerShake(6.0f, 0.3f);
                }
            }
        });

    systemCherryBomb(m_gs, gameDt);
    systemJalapeno(m_gs, gameDt);

    systemProjectileMove(m_gs, gameDt);
    systemTorchwood(m_gs, gameDt);

    // Snapshot projectile positions for splat effects
    struct ProjSnap { SF3::Entity e; float x, y; ProjectileType type; };
    std::vector<ProjSnap> projSnaps;
    m_gs.world.each<Projectile, WorldPos>(
        [&](SF3::Entity e, Projectile& proj, WorldPos& wp) {
            projSnaps.push_back({e, wp.pos.x, wp.pos.y, proj.type});
        });

    systemProjectileCollision(m_gs, gameDt);

    // Spawn splat particles for destroyed projectiles
    for (auto& ps : projSnaps) {
        bool destroyed = false;
        for (auto de : m_gs.pendingDestroy) {
            if (de == ps.e) { destroyed = true; break; }
        }
        if (destroyed) {
            spawnSplatParticles(ps.x, ps.y, ps.type);
        }
    }

    systemZombieSpawn(m_gs, gameDt);
    systemZombieSpecial(m_gs, gameDt);
    systemZombieMove(m_gs, gameDt);
    systemZombieEat(m_gs, gameDt);

    systemSunFall(m_gs, gameDt);
    systemSkySunDrop(m_gs, gameDt);

    systemLawnmower(m_gs, gameDt);
    systemFreeze(m_gs, gameDt);
    systemBossAI(m_gs, gameDt);
    systemPowerUps(m_gs, gameDt);
    systemAnimation(m_gs, gameDt);
    systemDrops(m_gs, gameDt);

    systemWinLose(m_gs, gameDt);

    // Spawn death particles for newly killed zombies
    int newKills = m_gs.totalZombiesKilled - prevKilled;
    if (newKills > 0) {
        for (auto e : m_gs.pendingDestroy) {
            if (m_gs.world.alive(e) && m_gs.world.has<Zombie>(e) && m_gs.world.has<WorldPos>(e)) {
                auto& wp = m_gs.world.get<WorldPos>(e);
                spawnDeathParticles(wp.pos.x, wp.pos.y);
            }
        }
    }

    m_gs.flushDestroyQueue();

    // Update camera shake
    if (m_shakeTimer > 0) {
        m_shakeTimer -= dt;
        float t = m_shakeTimer / 0.4f;
        m_shakeOffsetX = (std::rand() % 100 - 50) * 0.01f * m_shakeIntensity * t;
        m_shakeOffsetY = (std::rand() % 100 - 50) * 0.01f * m_shakeIntensity * t;
    } else {
        m_shakeOffsetX = 0;
        m_shakeOffsetY = 0;
    }

    updateParticles(dt);
}

void GameScene::render() {
    renderSky();
    renderGrid();
    renderProjectileTrails();
    renderEntities();
    renderDrops();
    renderParticles();

    // Explosion flash overlay
    if (m_shakeTimer > 0) {
        float t = m_shakeTimer / 0.4f;
        uint8_t alpha = static_cast<uint8_t>(t * 40);
        SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(255, 200, 100, alpha));
    }

    // Night / Fog overlays (after entities, before HUD)
    if (m_gs.levelDef && m_gs.levelDef->isNight) {
        renderNightOverlay();
    }
    if (m_gs.levelDef && m_gs.levelDef->hasFog) {
        renderFogOverlay();
    }

    renderHUD();

    // Zombie approach warnings (right edge indicators for each lane with zombies)
    m_gs.world.each<Zombie, WorldPos, GridPos>(
        [&](SF3::Entity, Zombie&, WorldPos& wp, GridPos& gp) {
            if (wp.pos.x > GRID_OFFSET_X + GRID_COLS * CELL_W) {
                float iy = cellCenterY(gp.row);
                float ix = SCREEN_W - 12;
                float pulse = std::sin(SF3::App::instance().elapsedTime() * 6.0f) * 0.3f + 0.7f;
                uint8_t a = static_cast<uint8_t>(pulse * 200);
                SF3::Graphics::drawCircleFilled(ix, iy, 5, SF3::Color(255, 60, 60, a));
                SF3::Graphics::drawCircleFilled(ix, iy, 2, SF3::Color(255, 200, 100, a));
            }
        });

    // Wave progress bar (below HUD)
    if (m_gs.levelDef && !m_gs.levelComplete && !m_gs.levelFailed) {
        int totalWaves = static_cast<int>(m_gs.levelDef->waves.size());
        if (totalWaves > 0) {
            float barW = 120.0f;
            float barH = 6.0f;
            float barX = SCREEN_W - barW - 10;
            float barY = HUD::BAR_H + 4;
            float progress = static_cast<float>(m_gs.currentWave) / totalWaves;

            SF3::Graphics::drawRect({barX, barY, barW, barH}, SF3::Color(40, 40, 40, 180));
            SF3::Graphics::drawRect({barX, barY, barW * progress, barH},
                SF3::Color(200, 80, 60, 220));
            SF3::Graphics::drawRectOutline({barX, barY, barW, barH},
                SF3::Color(100, 80, 60, 150));

            // Flag markers for flag waves
            for (int w = 0; w < totalWaves; w++) {
                if (m_gs.levelDef->waves[w].isFlagWave) {
                    float fx = barX + (static_cast<float>(w) / totalWaves) * barW;
                    SF3::Graphics::drawLine(fx, barY - 2, fx, barY + barH + 2,
                        SF3::Color(255, 50, 50, 150), 1);
                }
            }

            auto& fm = FontManager::instance();
            char waveBuf[32];
            std::snprintf(waveBuf, sizeof(waveBuf), u8"波次 %d/%d", m_gs.currentWave, totalWaves);
            fm.drawSmall(waveBuf, barX, barY + barH + 2, SF3::Color(180, 160, 140, 200));
        }
    }

    renderComboDisplay();
    renderPowerUpIndicators();
    renderWaveAnnouncement();

    if (m_gs.levelComplete || m_gs.levelFailed) {
        renderGameOver();
    }

    if (m_gs.paused) {
        renderPauseOverlay();
    }

    // Selected plant follows mouse
    if (m_gs.selectedPlant >= 0) {
        auto mpos = SF3::Input::mousePos();
        auto type = m_hud.getSelectedPlantType();
        Renderer::drawPlant(type, mpos.x, mpos.y, 0, 1.0f);
    }
}

void GameScene::renderSky() {
    // Sky gradient: darker blue at top, lighter at horizon
    int bands = 8;
    float bandH = GRID_OFFSET_Y / bands;
    for (int i = 0; i < bands; i++) {
        float t = static_cast<float>(i) / (bands - 1);
        uint8_t r = static_cast<uint8_t>(100 + t * 55);
        uint8_t g = static_cast<uint8_t>(160 + t * 60);
        uint8_t b = static_cast<uint8_t>(220 + t * 30);
        SF3::Graphics::drawRect({0, i * bandH, SCREEN_W, bandH + 1}, SF3::Color(r, g, b));
    }

    // Below grid area - earth tone
    float gridBottom = GRID_OFFSET_Y + GRID_ROWS * CELL_H;
    SF3::Graphics::drawRect({0, gridBottom, SCREEN_W, SCREEN_H - gridBottom},
        SF3::Color(80, 60, 30));

    // Animated clouds
    auto drawCloud = [](float cx, float cy, float scale) {
        SF3::Color c1(255, 255, 255, 160);
        SF3::Color c2(230, 240, 250, 120);
        SF3::Graphics::drawCircleFilled(cx, cy, 18 * scale, c1);
        SF3::Graphics::drawCircleFilled(cx - 15 * scale, cy + 3, 14 * scale, c1);
        SF3::Graphics::drawCircleFilled(cx + 16 * scale, cy + 2, 15 * scale, c1);
        SF3::Graphics::drawCircleFilled(cx + 8 * scale, cy - 8 * scale, 12 * scale, c2);
    };

    drawCloud(m_cloudOffset, 25, 1.2f);
    drawCloud(std::fmod(m_cloudOffset + 350, SCREEN_W + 200) - 100, 40, 0.9f);
    drawCloud(std::fmod(m_cloudOffset * 0.7f + 600, SCREEN_W + 200) - 100, 18, 1.0f);
}

void GameScene::renderHUD() {
    m_hud.render();

    // Game speed indicator
    if (m_gs.gameSpeed > 1.5f) {
        auto& fm = FontManager::instance();
        fm.drawSmall(u8"▶▶ 2x", SCREEN_W * 0.5f - 20, 52, SF3::Color(255, 200, 50));
    }
}

void GameScene::renderWaveAnnouncement() {
    if (m_waveAnnounceTimer <= 0) return;

    auto& fm = FontManager::instance();
    float t = m_waveAnnounceTimer;

    // Fade in (first 0.5s), stay, fade out (last 0.5s)
    float alpha = 1.0f;
    if (t > 2.5f) alpha = (3.0f - t) * 2.0f;  // fade in
    if (t < 0.5f) alpha = t * 2.0f;              // fade out
    alpha = std::max(0.0f, std::min(1.0f, alpha));

    uint8_t a = static_cast<uint8_t>(alpha * 200);
    uint8_t ta = static_cast<uint8_t>(alpha * 255);

    float cx = SCREEN_W * 0.5f;
    float cy = SCREEN_H * 0.35f;

    // Banner background
    SF3::Graphics::drawRect({cx - 180, cy - 25, 360, 55}, SF3::Color(140, 30, 30, a));
    SF3::Graphics::drawRectOutline({cx - 180, cy - 25, 360, 55}, SF3::Color(200, 50, 50, a));

    // Banner text
    fm.drawTextCenteredShadow(u8"大波殭屍來襲！", cx, cy - 18,
        SF3::Color(255, 220, 100, ta));

    // Wave number
    char waveBuf[32];
    std::snprintf(waveBuf, sizeof(waveBuf), u8"第 %d 波", m_announcedWave + 1);
    fm.drawSmallCentered(waveBuf, cx, cy + 16, SF3::Color(200, 180, 150, ta));
}

void GameScene::renderPauseOverlay() {
    // Dim background
    SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(0, 0, 0, 140));

    auto& fm = FontManager::instance();
    float cx = SCREEN_W * 0.5f;
    float cy = SCREEN_H * 0.4f;

    // Pause panel
    SF3::Graphics::drawRect({cx - 130, cy - 50, 260, 140}, SF3::Color(40, 60, 30, 220));
    SF3::Graphics::drawRectOutline({cx - 130, cy - 50, 260, 140}, SF3::Color(100, 160, 80));

    fm.drawTextCenteredShadow(u8"暫停", cx, cy - 40, SF3::Color(200, 255, 180));

    fm.drawSmallCentered(u8"ESC - 繼續遊戲", cx, cy + 10, SF3::Color(180, 200, 160));
    fm.drawSmallCentered(u8"Q - 回到主選單", cx, cy + 35, SF3::Color(180, 200, 160));
    fm.drawSmallCentered(u8"Space - 加速 (2x)", cx, cy + 60, SF3::Color(180, 200, 160));
}

void GameScene::renderGrid() {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            auto mpos = SF3::Input::mousePos();
            int hr = rowFromY(mpos.y);
            int hc = colFromX(mpos.x);
            bool highlight = (r == hr && c == hc && (m_gs.selectedPlant >= 0 || m_gs.shovelActive));
            Renderer::drawGrassCell(r, c, highlight);
        }
    }

    // Right-side dirt path (where zombies come from)
    float pathX = GRID_OFFSET_X + GRID_COLS * CELL_W;
    SF3::Graphics::drawRect({pathX, GRID_OFFSET_Y, 80, GRID_ROWS * CELL_H},
        SF3::Color(140, 110, 60));
    // Dirt texture detail
    for (int r = 0; r < GRID_ROWS; r++) {
        float py = GRID_OFFSET_Y + r * CELL_H;
        // Pebbles
        SF3::Graphics::drawCircleFilled(pathX + 15, py + 25, 3, SF3::Color(120, 95, 50));
        SF3::Graphics::drawCircleFilled(pathX + 50, py + 60, 2, SF3::Color(130, 100, 55));
        SF3::Graphics::drawCircleFilled(pathX + 35, py + 80, 2.5f, SF3::Color(125, 98, 52));
        // Lane divider line
        SF3::Graphics::drawLine(pathX, py + CELL_H, pathX + 80, py + CELL_H,
            SF3::Color(110, 85, 45, 60), 1);
    }
    // Left edge shadow
    SF3::Graphics::drawRect({pathX, GRID_OFFSET_Y, 3, GRID_ROWS * CELL_H},
        SF3::Color(100, 80, 40, 80));
}

void GameScene::renderEntities() {
    // Shadows pass — draw all shadows first so they appear under entities
    m_gs.world.each<Plant, WorldPos>(
        [&](SF3::Entity, Plant&, WorldPos& wp) {
            SF3::Graphics::drawCircleFilled(wp.pos.x, wp.pos.y + 28,
                16, SF3::Color(0, 0, 0, 25));
        });
    m_gs.world.each<Zombie, WorldPos>(
        [&](SF3::Entity, Zombie& z, WorldPos& wp) {
            float r = (z.type == ZombieType::Gargantuar) ? 22.0f :
                      (z.type == ZombieType::Imp) ? 8.0f : 14.0f;
            SF3::Graphics::drawCircleFilled(wp.pos.x, wp.pos.y + 30,
                r, SF3::Color(0, 0, 0, 20));
        });

    // Lawnmowers
    m_gs.world.each<Lawnmower, WorldPos>(
        [&](SF3::Entity, Lawnmower& lm, WorldPos& wp) {
            Renderer::drawLawnmower(wp.pos.x, wp.pos.y, lm.triggered);
        });

    // Plants
    m_gs.world.each<Plant, WorldPos, SpriteAnim, Health>(
        [&](SF3::Entity e, Plant& plant, WorldPos& wp, SpriteAnim& anim, Health& h) {
            float extra = static_cast<float>(h.current) / h.max;
            if (plant.type == PlantType::PotatoMine && m_gs.world.has<PotatoMineState>(e)) {
                extra = m_gs.world.get<PotatoMineState>(e).armed ? 1.0f : 0.0f;
            }
            if (plant.type == PlantType::Chomper && m_gs.world.has<ChomperState>(e)) {
                extra = m_gs.world.get<ChomperState>(e).eating ? 0.0f : 1.0f;
            }
            if (plant.type == PlantType::CherryBomb && m_gs.world.has<ExplosionEffect>(e)) {
                extra = m_gs.world.get<ExplosionEffect>(e).timer;
            }
            if (plant.type == PlantType::Jalapeno && m_gs.world.has<ExplosionEffect>(e)) {
                extra = m_gs.world.get<ExplosionEffect>(e).timer;
            }
            Renderer::drawPlant(plant.type, wp.pos.x, wp.pos.y, anim.frameIndex, extra);

            // Damage flash
            if (h.damageFlash > 0) {
                SF3::Graphics::drawRect({wp.pos.x - 20, wp.pos.y - 25, 40, 50},
                    SF3::Color(255, 0, 0, 60));
            }
            // Plant HP bar when damaged
            float plantHpPct = static_cast<float>(h.current) / h.max;
            if (plantHpPct < 1.0f && plantHpPct > 0.0f) {
                float barW = 28.0f;
                float barH = 3.0f;
                float barX = wp.pos.x - barW * 0.5f;
                float barY = wp.pos.y + 28;
                SF3::Graphics::drawRect({barX, barY, barW, barH}, SF3::Color(60, 0, 0));
                SF3::Color hpc = (plantHpPct > 0.5f) ? SF3::Color(80, 200, 50) :
                                 (plantHpPct > 0.25f) ? SF3::Color(220, 180, 40) :
                                 SF3::Color(220, 50, 40);
                SF3::Graphics::drawRect({barX, barY, barW * plantHpPct, barH}, hpc);
            }
        });

    // Zombies
    m_gs.world.each<Zombie, WorldPos, SpriteAnim, Health>(
        [&](SF3::Entity e, Zombie& zombie, WorldPos& wp, SpriteAnim& anim, Health& h) {
            float hpPct = static_cast<float>(h.current) / h.max;
            float armorPct = 0.0f;
            bool special = false;

            if (m_gs.world.has<Armor>(e)) {
                auto& armor = m_gs.world.get<Armor>(e);
                armorPct = (armor.max > 0) ? static_cast<float>(armor.current) / armor.max : 0.0f;
            }
            if (m_gs.world.has<ZombieSpecial>(e)) {
                auto& spec = m_gs.world.get<ZombieSpecial>(e);
                special = spec.isAngry || spec.hasJumped || spec.hasSummoned;
            }

            Renderer::drawZombie(zombie.type, wp.pos.x, wp.pos.y, anim.frameIndex,
                                  hpPct, armorPct, special);

            // Freeze tint
            if (m_gs.world.has<Freezable>(e)) {
                auto& fr = m_gs.world.get<Freezable>(e);
                if (fr.slowFactor < 1.0f) {
                    SF3::Graphics::drawRect({wp.pos.x - 15, wp.pos.y - 30, 30, 60},
                        SF3::Color(100, 180, 255, 40));
                }
            }

            // Health bar
            if (hpPct < 1.0f) {
                float barW = 30.0f;
                float barH = 4.0f;
                float barX = wp.pos.x - barW * 0.5f;
                float barY = wp.pos.y - 35;
                SF3::Graphics::drawRect({barX, barY, barW, barH}, SF3::Color(60, 0, 0));
                SF3::Graphics::drawRect({barX, barY, barW * hpPct, barH}, SF3::Color(0, 200, 0));
            }
        });

    // Projectiles
    m_gs.world.each<Projectile, WorldPos>(
        [&](SF3::Entity, Projectile& proj, WorldPos& wp) {
            switch (proj.type) {
                case ProjectileType::Pea: Renderer::drawPea(wp.pos.x, wp.pos.y); break;
                case ProjectileType::SnowPea: Renderer::drawSnowPeaBullet(wp.pos.x, wp.pos.y); break;
                case ProjectileType::FirePea: Renderer::drawFirePea(wp.pos.x, wp.pos.y); break;
            }
        });

    // Suns
    m_gs.world.each<Sun, WorldPos>(
        [&](SF3::Entity, Sun& sun, WorldPos& wp) {
            float scale = sun.collecting ? 0.7f : 1.0f;
            if (sun.lifetime < 2.0f) scale *= (sun.lifetime / 2.0f); // fade out
            Renderer::drawSun(wp.pos.x, wp.pos.y, scale);
        });
}

void GameScene::renderGameOver() {
    // Progressive dimming based on timer
    float dimAlpha = std::min(1.0f, m_gameOverTimer * 0.8f);
    uint8_t da = static_cast<uint8_t>(dimAlpha * 150);
    SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(0, 0, 0, da));
    auto& fm = FontManager::instance();

    float cx = SCREEN_W * 0.5f;
    float cy = SCREEN_H * 0.4f;

    // Pulsing scale effect
    float pulse = 1.0f + std::sin(m_gameOverTimer * 4.0f) * 0.05f;
    float bannerW = 250 * pulse;
    float bannerH = 100 * pulse;

    if (m_gs.levelComplete) {
        SF3::Graphics::drawRect({cx - bannerW * 0.5f, cy - bannerH * 0.5f, bannerW, bannerH},
            SF3::Color(50, 120, 35, 230));
        SF3::Graphics::drawRectOutline({cx - bannerW * 0.5f, cy - bannerH * 0.5f, bannerW, bannerH},
            SF3::Color(100, 200, 60));
        fm.drawTextCenteredShadow(u8"勝利！", cx, cy - 30, SF3::Color(255, 255, 200));

        // Show stats preview
        char statBuf[64];
        std::snprintf(statBuf, sizeof(statBuf), u8"消滅 %d 隻殭屍", m_gs.totalZombiesKilled);
        fm.drawSmallCentered(statBuf, cx, cy + 5, SF3::Color(200, 230, 180));
        std::snprintf(statBuf, sizeof(statBuf), u8"獲得 %d 金幣", m_gs.coinsEarned);
        fm.drawSmallCentered(statBuf, cx, cy + 25, SF3::Color(255, 220, 100));
    } else {
        SF3::Graphics::drawRect({cx - bannerW * 0.5f, cy - bannerH * 0.5f, bannerW, bannerH},
            SF3::Color(120, 35, 35, 230));
        SF3::Graphics::drawRectOutline({cx - bannerW * 0.5f, cy - bannerH * 0.5f, bannerW, bannerH},
            SF3::Color(200, 60, 60));
        fm.drawTextCenteredShadow(u8"失敗...", cx, cy - 20, SF3::Color(255, 200, 200));
        fm.drawSmallCentered(u8"殭屍突破了防線！", cx, cy + 10, SF3::Color(200, 150, 150));
    }
}

void GameScene::renderDrops() {
    m_gs.world.each<Drop, WorldPos>(
        [&](SF3::Entity, Drop& drop, WorldPos& wp) {
            float scale = 1.0f;
            if (drop.collecting) scale = 0.7f;
            if (drop.lifetime < 2.0f) scale *= (drop.lifetime / 2.0f);
            Renderer::drawDrop(drop.type, wp.pos.x, wp.pos.y, scale);
        });
}

void GameScene::renderNightOverlay() {
    // Dark overlay covering the whole screen
    SF3::Graphics::drawRect({0, 0, SCREEN_W, SCREEN_H}, SF3::Color(10, 10, 40, 140));

    // Light circles around each plant (they illuminate the area)
    m_gs.world.each<Plant, WorldPos>(
        [&](SF3::Entity, Plant&, WorldPos& wp) {
            // Draw concentric circles with decreasing alpha to create a light effect
            for (int i = 4; i >= 0; i--) {
                float r = 40.0f + i * 15.0f;
                uint8_t a = static_cast<uint8_t>((4 - i) * 8);
                SF3::Graphics::drawCircleFilled(wp.pos.x, wp.pos.y, r,
                    SF3::Color(255, 255, 200, a));
            }
        });

    // Torchwood provides bigger light radius
    m_gs.world.each<Plant, WorldPos>(
        [&](SF3::Entity, Plant& p, WorldPos& wp) {
            if (p.type != PlantType::Torchwood) return;
            for (int i = 5; i >= 0; i--) {
                float r = 50.0f + i * 20.0f;
                uint8_t a = static_cast<uint8_t>((5 - i) * 6);
                SF3::Graphics::drawCircleFilled(wp.pos.x, wp.pos.y, r,
                    SF3::Color(255, 200, 100, a));
            }
        });

    // Lawnmower area lights
    m_gs.world.each<Lawnmower, WorldPos>(
        [&](SF3::Entity, Lawnmower& lm, WorldPos& wp) {
            if (lm.triggered) return;
            SF3::Graphics::drawCircleFilled(wp.pos.x, wp.pos.y, 30,
                SF3::Color(255, 255, 200, 15));
        });
}

void GameScene::renderFogOverlay() {
    // Fog gradient from right side - thicker on the right, fading to left
    float fogStartX = GRID_OFFSET_X + GRID_COLS * CELL_W * 0.4f;
    float fogEndX = SCREEN_W;
    int fogBands = 10;
    float bandW = (fogEndX - fogStartX) / fogBands;

    for (int i = 0; i < fogBands; i++) {
        float t = static_cast<float>(i) / (fogBands - 1);
        uint8_t alpha = static_cast<uint8_t>(t * t * 180); // quadratic fade-in
        float x = fogStartX + i * bandW;
        SF3::Graphics::drawRect({x, GRID_OFFSET_Y, bandW + 1, GRID_ROWS * CELL_H},
            SF3::Color(180, 200, 220, alpha));
    }

    // Animated fog wisps
    float time = SF3::App::instance().elapsedTime();
    for (int i = 0; i < 5; i++) {
        float wx = fogStartX + 100 + std::sin(time * 0.3f + i * 1.5f) * 60;
        float wy = GRID_OFFSET_Y + 50 + i * 90;
        SF3::Graphics::drawCircleFilled(wx, wy, 30 + std::sin(time + i) * 10,
            SF3::Color(200, 220, 240, 40));
    }
}

void GameScene::renderComboDisplay() {
    if (m_gs.comboDisplayTimer <= 0 || m_gs.comboCount < 2) return;

    auto& fm = FontManager::instance();
    float alpha = std::min(1.0f, m_gs.comboDisplayTimer);
    uint8_t a = static_cast<uint8_t>(alpha * 255);

    // Combo text in upper-right area
    float cx = SCREEN_W - 100;
    float cy = 90;

    char comboBuf[32];
    std::snprintf(comboBuf, sizeof(comboBuf), u8"COMBO x%d", m_gs.comboCount);

    // Combo color escalates with count
    SF3::Color comboColor;
    if (m_gs.comboCount >= 10)
        comboColor = SF3::Color(255, 50, 50, a);    // red
    else if (m_gs.comboCount >= 5)
        comboColor = SF3::Color(255, 180, 50, a);   // orange
    else
        comboColor = SF3::Color(255, 255, 100, a);  // yellow

    // Background
    SF3::Graphics::drawRect({cx - 60, cy - 5, 120, 30}, SF3::Color(0, 0, 0, a / 3));
    fm.drawTextCenteredShadow(comboBuf, cx, cy, comboColor);
}

void GameScene::renderPowerUpIndicators() {
    auto& fm = FontManager::instance();
    float y = HUD::BAR_H + 5;
    float x = 5;

    if (m_gs.plantBoostTimer > 0) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), u8"攻擊加速 %.0fs", m_gs.plantBoostTimer);
        SF3::Graphics::drawRect({x, y, 120, 18}, SF3::Color(50, 150, 50, 180));
        fm.drawSmall(buf, x + 4, y + 1, SF3::Color(200, 255, 200));
        y += 22;
    }

    if (m_gs.freezeAllTimer > 0) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), u8"全體凍結 %.0fs", m_gs.freezeAllTimer);
        SF3::Graphics::drawRect({x, y, 120, 18}, SF3::Color(50, 100, 200, 180));
        fm.drawSmall(buf, x + 4, y + 1, SF3::Color(200, 220, 255));
        y += 22;
    }
}

// ============================================================================
// Particle Effects
// ============================================================================

void GameScene::spawnExplosionParticles(float cx, float cy, int count,
                                         uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < count; i++) {
        float angle = (std::rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + std::rand() % 200;
        Particle p;
        p.x = cx;
        p.y = cy;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed - 50.0f;
        p.life = 0.3f + (std::rand() % 50) * 0.01f;
        p.maxLife = p.life;
        p.r = r;
        p.g = g;
        p.b = b;
        p.size = 2.0f + (std::rand() % 4);
        m_particles.push_back(p);
    }
}

void GameScene::spawnDeathParticles(float cx, float cy) {
    spawnExplosionParticles(cx, cy, 8, 150, 180, 130);
}

void GameScene::spawnSplatParticles(float cx, float cy, ProjectileType type) {
    uint8_t r, g, b;
    switch (type) {
        case ProjectileType::Pea:     r = 100; g = 200; b = 50; break;
        case ProjectileType::SnowPea: r = 120; g = 190; b = 230; break;
        case ProjectileType::FirePea: r = 255; g = 150; b = 30; break;
    }
    for (int i = 0; i < 5; i++) {
        float angle = (std::rand() % 360) * 3.14159f / 180.0f;
        float speed = 30.0f + std::rand() % 80;
        Particle p;
        p.x = cx;
        p.y = cy;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed - 30.0f;
        p.life = 0.15f + (std::rand() % 20) * 0.01f;
        p.maxLife = p.life;
        p.r = r; p.g = g; p.b = b;
        p.size = 2.0f + (std::rand() % 3);
        m_particles.push_back(p);
    }
}

void GameScene::spawnCollectSparkle(float cx, float cy, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f / 4.0f + (std::rand() % 20) * 0.01f;
        float speed = 40.0f + std::rand() % 60;
        Particle p;
        p.x = cx;
        p.y = cy;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        p.life = 0.25f + (std::rand() % 15) * 0.01f;
        p.maxLife = p.life;
        p.r = r; p.g = g; p.b = b;
        p.size = 1.5f + (std::rand() % 3);
        m_particles.push_back(p);
    }
}

void GameScene::updateParticles(float dt) {
    for (auto& p : m_particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vy += 200.0f * dt; // gravity
        p.life -= dt;
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.life <= 0; }),
        m_particles.end());
}

void GameScene::renderParticles() {
    for (auto& p : m_particles) {
        float alpha = p.life / p.maxLife;
        uint8_t a = static_cast<uint8_t>(alpha * 255);
        SF3::Graphics::drawCircleFilled(p.x + m_shakeOffsetX, p.y + m_shakeOffsetY,
            p.size * alpha, SF3::Color(p.r, p.g, p.b, a));
    }
}

void GameScene::renderProjectileTrails() {
    m_gs.world.each<Projectile, WorldPos>(
        [&](SF3::Entity, Projectile& proj, WorldPos& wp) {
            switch (proj.type) {
                case ProjectileType::Pea: {
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 8, wp.pos.y, 3,
                        SF3::Color(100, 200, 50, 60));
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 14, wp.pos.y, 2,
                        SF3::Color(100, 200, 50, 30));
                    break;
                }
                case ProjectileType::SnowPea: {
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 8, wp.pos.y, 3,
                        SF3::Color(120, 190, 230, 60));
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 14, wp.pos.y, 2,
                        SF3::Color(120, 190, 230, 30));
                    // Ice sparkle
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 5, wp.pos.y - 3, 1.5f,
                        SF3::Color(200, 230, 255, 80));
                    break;
                }
                case ProjectileType::FirePea: {
                    // Longer, brighter trail
                    for (int i = 1; i <= 4; i++) {
                        float tx = wp.pos.x - i * 6;
                        float r = 5.0f - i * 0.8f;
                        uint8_t a = static_cast<uint8_t>(100 - i * 20);
                        SF3::Graphics::drawCircleFilled(tx, wp.pos.y, r,
                            SF3::Color(255, 150, 30, a));
                    }
                    SF3::Graphics::drawCircleFilled(wp.pos.x - 4, wp.pos.y - 2, 2,
                        SF3::Color(255, 255, 150, 100));
                    break;
                }
            }
        });
}

void GameScene::triggerShake(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_shakeTimer = duration;
}

// ============================================================================
// Game Over Scene
// ============================================================================

GameOverScene::GameOverScene() : Scene("gameover") {}
void GameOverScene::onCreate() {}

void GameOverScene::update(SF3::f32 dt) {
    (void)dt;
    if (SF3::Input::mousePressed(SF3::MouseButton::Left) ||
        SF3::Input::keyPressed(SF3::Key::Space) ||
        SF3::Input::keyPressed(SF3::Key::Return)) {
        switchTo("levelselect", SF3::SceneTransition::Fade(0.3f));
    }
    if (SF3::Input::keyPressed(SF3::Key::Escape)) {
        switchTo("menu", SF3::SceneTransition::Fade(0.3f));
    }
}

void GameOverScene::render() {
    // Themed gradient background
    if (wasVictory) {
        for (int i = 0; i < 6; i++) {
            float t = static_cast<float>(i) / 5.0f;
            uint8_t r = static_cast<uint8_t>(30 + t * 25);
            uint8_t g = static_cast<uint8_t>(60 + t * 40);
            uint8_t b2 = static_cast<uint8_t>(20 + t * 20);
            SF3::Graphics::drawRect({0, i * 100.0f, SCREEN_W, 101}, SF3::Color(r, g, b2));
        }
    } else {
        for (int i = 0; i < 6; i++) {
            float t = static_cast<float>(i) / 5.0f;
            uint8_t r = static_cast<uint8_t>(30 + t * 20);
            uint8_t g = static_cast<uint8_t>(10 + t * 15);
            uint8_t b2 = static_cast<uint8_t>(10 + t * 15);
            SF3::Graphics::drawRect({0, i * 100.0f, SCREEN_W, 101}, SF3::Color(r, g, b2));
        }
    }

    auto& fm = FontManager::instance();
    float cx = SCREEN_W * 0.5f;
    float t = SF3::App::instance().elapsedTime();

    // Title banner with decorative border
    SF3::Color bannerC = wasVictory ? SF3::Color(50, 120, 35, 230) : SF3::Color(120, 35, 35, 230);
    SF3::Graphics::drawRect({cx - 180, 80, 360, 70}, bannerC);
    SF3::Graphics::drawRect({cx - 180, 80, 360, 4},
        wasVictory ? SF3::Color(100, 200, 60) : SF3::Color(200, 60, 60));
    SF3::Graphics::drawRect({cx - 180, 146, 360, 4},
        wasVictory ? SF3::Color(100, 200, 60) : SF3::Color(200, 60, 60));
    fm.drawTextCenteredShadow(
        wasVictory ? u8"關卡通過！" : u8"殭屍入侵了...",
        cx, 95, SF3::Color(255, 255, 230));

    // Stats panel
    SF3::Graphics::drawRect({cx - 140, 180, 280, 220}, SF3::Color(25, 25, 25, 200));
    SF3::Graphics::drawRectOutline({cx - 140, 180, 280, 220}, SF3::Color(80, 80, 80, 150));

    // Zombies killed
    fm.drawSmall(u8"消滅殭屍", cx - 120, 195, SF3::Color(180, 180, 180));
    char killBuf[32];
    std::snprintf(killBuf, sizeof(killBuf), "%d", zombiesKilled);
    fm.drawTextRight(killBuf, cx + 120, 190, SF3::Color(255, 100, 80));

    float killBarW = std::min(240.0f, zombiesKilled * 12.0f);
    SF3::Graphics::drawRect({cx - 120, 222, 240, 14}, SF3::Color(40, 20, 20));
    SF3::Graphics::drawRect({cx - 120, 222, killBarW, 14}, SF3::Color(200, 60, 50));
    SF3::Graphics::drawRectOutline({cx - 120, 222, 240, 14}, SF3::Color(80, 40, 40));

    // Coins earned
    fm.drawSmall(u8"獲得金幣", cx - 120, 255, SF3::Color(180, 180, 180));
    Renderer::drawSunIcon(cx + 80, 260, 18);
    char coinBuf[32];
    std::snprintf(coinBuf, sizeof(coinBuf), "%d", earnedCoins);
    fm.drawTextRight(coinBuf, cx + 120, 250, SF3::Color(255, 220, 50));

    float coinBarW = std::min(240.0f, earnedCoins * 2.4f);
    SF3::Graphics::drawRect({cx - 120, 280, 240, 14}, SF3::Color(40, 35, 10));
    SF3::Graphics::drawRect({cx - 120, 280, coinBarW, 14}, SF3::Color(255, 200, 50));
    SF3::Graphics::drawRectOutline({cx - 120, 280, 240, 14}, SF3::Color(80, 70, 20));

    // Total coins
    fm.drawSmall(u8"總金幣", cx - 120, 310, SF3::Color(150, 150, 150));
    char totalBuf[32];
    std::snprintf(totalBuf, sizeof(totalBuf), "%d", ShopScene::coins + (wasVictory ? earnedCoins : 0));
    fm.drawTextRight(totalBuf, cx + 120, 308, SF3::Color(200, 180, 100));

    // Max combo
    if (maxCombo >= 2) {
        fm.drawSmall(u8"最大連殺", cx - 120, 340, SF3::Color(180, 180, 180));
        char comboBuf2[32];
        std::snprintf(comboBuf2, sizeof(comboBuf2), "x%d", maxCombo);
        SF3::Color cc = (maxCombo >= 10) ? SF3::Color(255, 80, 80) :
                        (maxCombo >= 5) ? SF3::Color(255, 180, 50) : SF3::Color(255, 255, 100);
        fm.drawTextRight(comboBuf2, cx + 120, 338, cc);
    }

    // Rating stars (based on kills and victory)
    if (wasVictory) {
        int stars = (zombiesKilled > 20) ? 3 : (zombiesKilled > 10 ? 2 : 1);
        fm.drawSmall(u8"評價", cx - 120, 370, SF3::Color(180, 180, 180));
        for (int s = 0; s < 3; s++) {
            float sx = cx + 30 + s * 30;
            float sy = 370;
            SF3::Color starC = (s < stars) ? SF3::Color(255, 220, 50) : SF3::Color(60, 60, 60);
            SF3::Graphics::drawCircleFilled(sx, sy + 8, 10, starC);
            if (s < stars) {
                SF3::Graphics::drawCircleFilled(sx, sy + 5, 5, SF3::Color(255, 240, 120));
            }
        }
    }

    // Continue prompt (blinking)
    if (static_cast<int>(t * 3) % 2 == 0) {
        fm.drawTextCentered(u8"點擊繼續", cx, 460, SF3::Color(180, 180, 180));
    }
    fm.drawSmallCentered(u8"ESC - 回到主選單", cx, 490, SF3::Color(120, 120, 120));

    // Decorative characters
    if (wasVictory) {
        Renderer::drawSunflower(80, 400, static_cast<int>(t * 3) % 2);
        Renderer::drawPeashooter(180, 440, static_cast<int>(t * 2) % 2);
        Renderer::drawSunflower(SCREEN_W - 80, 400, (static_cast<int>(t * 3) + 1) % 2);
        Renderer::drawWallNut(SCREEN_W - 160, 430, 1.0f);
        // Animated suns
        float sunBob = std::sin(t * 3.0f) * 6.0f;
        Renderer::drawSun(cx - 200, 120 + sunBob, 0.8f);
        Renderer::drawSun(cx + 200, 120 - sunBob, 0.8f);
    } else {
        Renderer::drawBasicZombie(cx - 120, 440, static_cast<int>(t * 2) % 4, 1.0f);
        Renderer::drawBasicZombie(cx + 120, 450, (static_cast<int>(t * 2) + 2) % 4, 1.0f);
        Renderer::drawConeZombie(cx, 430, static_cast<int>(t * 1.5f) % 4, 0.9f);
    }
}

} // namespace PVZ
