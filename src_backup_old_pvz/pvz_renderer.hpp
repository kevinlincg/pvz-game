#pragma once

#include <sf3.hpp>
#include "pvz_data.hpp"

namespace PVZ {

// Procedural pixel art renderer — all sprites drawn with primitives
class Renderer {
public:
    // Plants
    static void drawSunflower(float cx, float cy, int frame);
    static void drawPeashooter(float cx, float cy, int frame);
    static void drawWallNut(float cx, float cy, float hpPct);
    static void drawSnowPea(float cx, float cy, int frame);
    static void drawCherryBomb(float cx, float cy, float timer);
    static void drawPotatoMine(float cx, float cy, bool armed);
    static void drawRepeater(float cx, float cy, int frame);
    static void drawChomper(float cx, float cy, bool eating);
    static void drawJalapeno(float cx, float cy, float timer);
    static void drawTorchwood(float cx, float cy, int frame);

    static void drawPlant(PlantType type, float cx, float cy, int frame, float extra = 1.0f);

    // Zombies
    static void drawBasicZombie(float cx, float cy, int frame, float hpPct);
    static void drawConeZombie(float cx, float cy, int frame, float armorPct);
    static void drawBucketZombie(float cx, float cy, int frame, float armorPct);
    static void drawFlagZombie(float cx, float cy, int frame);
    static void drawNewspaperZombie(float cx, float cy, int frame, bool angry);
    static void drawPoleVaultZombie(float cx, float cy, int frame, bool jumped);
    static void drawFootballZombie(float cx, float cy, int frame, float hpPct);
    static void drawDancingZombie(float cx, float cy, int frame);
    static void drawImpZombie(float cx, float cy, int frame, float hpPct);
    static void drawGargantuarZombie(float cx, float cy, int frame, float hpPct);

    static void drawZombie(ZombieType type, float cx, float cy, int frame, float hpPct, float armorPct, bool special);

    // Projectiles
    static void drawPea(float x, float y);
    static void drawSnowPeaBullet(float x, float y);
    static void drawFirePea(float x, float y);

    // Sun
    static void drawSun(float cx, float cy, float scale = 1.0f);

    // Grid / Environment
    static void drawGrassCell(int row, int col, bool highlight);
    static void drawLawnmower(float x, float y, bool triggered);

    // Explosions / effects
    static void drawExplosion(float cx, float cy, float radius, float alpha);
    static void drawFireLane(int row, float progress);

    // HUD helpers
    static void drawSeedPacket(PlantType type, float x, float y, float w, float h,
                               bool selected, bool affordable, float cooldownPct);
    static void drawSunIcon(float x, float y, float size);
    static void drawShovel(float x, float y, float size, bool selected);

    // Drops
    static void drawDrop(DropType type, float cx, float cy, float scale = 1.0f);
};

} // namespace PVZ
