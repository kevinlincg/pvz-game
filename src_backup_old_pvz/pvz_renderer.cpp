#include "pvz_renderer.hpp"
#include <cmath>

using namespace SF3;

namespace PVZ {

// ============================================================================
// Helper
// ============================================================================

static void fillRect(float x, float y, float w, float h, Color c) {
    Rect r{x, y, w, h};
    Graphics::drawRect(r, c);
}

static void outlineRect(float x, float y, float w, float h, Color c) {
    Rect r{x, y, w, h};
    Graphics::drawRectOutline(r, c);
}

static void fillCircle(float cx, float cy, float radius, Color c) {
    Graphics::drawCircleFilled(cx, cy, radius, c);
}

static void outlineCircle(float cx, float cy, float radius, Color c) {
    Graphics::drawCircle(cx, cy, radius, c);
}

// ============================================================================
// Plants
// ============================================================================

void Renderer::drawSunflower(float cx, float cy, int frame) {
    // Stem
    fillRect(cx - 3, cy, 6, 30, Color(60, 140, 30));
    // Leaves
    fillRect(cx - 14, cy + 10, 12, 6, Color(70, 160, 40));
    fillRect(cx + 2, cy + 15, 12, 6, Color(70, 160, 40));
    // Petals (8 around center)
    float bobY = (frame % 2 == 0) ? -2.0f : 0.0f;
    Color petalC(255, 220, 50);
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f / 4.0f;
        float px = cx + std::cos(angle) * 14.0f;
        float py = (cy - 10 + bobY) + std::sin(angle) * 14.0f;
        fillCircle(px, py, 7, petalC);
    }
    // Center face
    fillCircle(cx, cy - 10 + bobY, 10, Color(120, 80, 20));
    // Eyes
    fillCircle(cx - 4, cy - 13 + bobY, 2, Color(30, 30, 30));
    fillCircle(cx + 4, cy - 13 + bobY, 2, Color(30, 30, 30));
    // Smile
    fillRect(cx - 4, cy - 7 + bobY, 8, 2, Color(30, 30, 30));
}

void Renderer::drawPeashooter(float cx, float cy, int frame) {
    // Stem
    fillRect(cx - 3, cy, 6, 30, Color(60, 140, 30));
    fillRect(cx - 12, cy + 15, 10, 6, Color(70, 160, 40));
    // Head
    fillCircle(cx, cy - 5, 16, Color(80, 180, 50));
    // Mouth (cannon)
    float recoil = (frame % 2 == 0) ? 2.0f : 0.0f;
    fillRect(cx + 8 + recoil, cy - 10, 14, 10, Color(60, 150, 40));
    fillRect(cx + 8 + recoil, cy - 12, 14, 4, Color(50, 130, 35));
    // Eyes
    fillCircle(cx - 2, cy - 10, 4, Color(255, 255, 255));
    fillCircle(cx - 1, cy - 10, 2, Color(30, 30, 30));
}

void Renderer::drawWallNut(float cx, float cy, float hpPct) {
    Color nutColor = (hpPct > 0.5f) ? Color(180, 140, 70) :
                     (hpPct > 0.25f) ? Color(150, 110, 50) : Color(120, 80, 30);
    // Body
    fillCircle(cx, cy - 5, 22, nutColor);
    outlineCircle(cx, cy - 5, 22, Color(100, 70, 20));
    // Face
    fillCircle(cx - 6, cy - 10, 3, Color(50, 30, 10));
    fillCircle(cx + 6, cy - 10, 3, Color(50, 30, 10));
    // Cracks based on damage
    if (hpPct < 0.66f) {
        Graphics::drawLine(cx - 8, cy - 2, cx - 2, cy + 8, Color(80, 50, 15), 2);
    }
    if (hpPct < 0.33f) {
        Graphics::drawLine(cx + 3, cy - 5, cx + 10, cy + 5, Color(80, 50, 15), 2);
    }
    // Mouth
    fillRect(cx - 5, cy, 10, 3, Color(50, 30, 10));
}

void Renderer::drawSnowPea(float cx, float cy, int frame) {
    // Stem
    fillRect(cx - 3, cy, 6, 30, Color(40, 100, 120));
    fillRect(cx - 12, cy + 15, 10, 6, Color(50, 120, 140));
    // Head
    fillCircle(cx, cy - 5, 16, Color(100, 180, 220));
    // Mouth cannon
    float recoil = (frame % 2 == 0) ? 2.0f : 0.0f;
    fillRect(cx + 8 + recoil, cy - 10, 14, 10, Color(80, 160, 200));
    fillRect(cx + 8 + recoil, cy - 12, 14, 4, Color(60, 140, 180));
    // Eyes
    fillCircle(cx - 2, cy - 10, 4, Color(220, 240, 255));
    fillCircle(cx - 1, cy - 10, 2, Color(30, 30, 80));
}

void Renderer::drawCherryBomb(float cx, float cy, float timer) {
    float pulse = 1.0f + std::sin(timer * 15.0f) * 0.15f;
    float r = 18.0f * pulse;
    // Left cherry
    fillCircle(cx - 8, cy - 2, r, Color(200, 30, 30));
    // Right cherry
    fillCircle(cx + 8, cy - 2, r, Color(220, 40, 40));
    // Stems
    Graphics::drawLine(cx - 8, cy - 20, cx - 4, cy - 5, Color(50, 120, 30), 3);
    Graphics::drawLine(cx + 8, cy - 20, cx + 4, cy - 5, Color(50, 120, 30), 3);
    // Angry eyes
    fillCircle(cx - 10, cy - 6, 3, Color(255, 255, 255));
    fillCircle(cx - 10, cy - 6, 1.5f, Color(30, 30, 30));
    fillCircle(cx + 6, cy - 6, 3, Color(255, 255, 255));
    fillCircle(cx + 6, cy - 6, 1.5f, Color(30, 30, 30));
    // Fuse
    if (timer > 0) {
        fillCircle(cx, cy - 22, 4, Color(255, 200, 50));
    }
}

void Renderer::drawPotatoMine(float cx, float cy, bool armed) {
    if (!armed) {
        // Underground - just dirt mound
        fillRect(cx - 15, cy + 10, 30, 15, Color(140, 100, 50));
        fillCircle(cx, cy + 10, 15, Color(120, 80, 40));
    } else {
        // Armed - visible potato
        fillCircle(cx, cy, 18, Color(180, 150, 80));
        outlineCircle(cx, cy, 18, Color(120, 90, 40));
        // Angry eyes
        fillCircle(cx - 6, cy - 4, 3, Color(255, 255, 255));
        fillCircle(cx - 5, cy - 4, 1.5f, Color(30, 30, 30));
        fillCircle(cx + 6, cy - 4, 3, Color(255, 255, 255));
        fillCircle(cx + 7, cy - 4, 1.5f, Color(30, 30, 30));
        // Mouth
        fillRect(cx - 4, cy + 4, 8, 3, Color(60, 30, 10));
    }
}

void Renderer::drawRepeater(float cx, float cy, int frame) {
    // Stem
    fillRect(cx - 3, cy, 6, 30, Color(60, 140, 30));
    fillRect(cx - 12, cy + 15, 10, 6, Color(70, 160, 40));
    // Head (double barrel)
    fillCircle(cx, cy - 5, 16, Color(60, 160, 40));
    float recoil = (frame % 2 == 0) ? 2.0f : 0.0f;
    // Upper barrel
    fillRect(cx + 8 + recoil, cy - 14, 16, 8, Color(50, 140, 35));
    // Lower barrel
    fillRect(cx + 8 + recoil, cy - 4, 16, 8, Color(50, 140, 35));
    // Eyes
    fillCircle(cx - 2, cy - 10, 4, Color(255, 255, 255));
    fillCircle(cx - 1, cy - 10, 2, Color(30, 30, 30));
}

void Renderer::drawChomper(float cx, float cy, bool eating) {
    // Stem
    fillRect(cx - 4, cy, 8, 30, Color(80, 50, 130));
    // Head
    float headY = eating ? cy + 5 : cy - 8;
    fillCircle(cx, headY, 20, Color(160, 80, 180));
    // Mouth
    if (eating) {
        fillRect(cx - 14, headY + 5, 28, 10, Color(120, 40, 60));
    } else {
        fillRect(cx - 14, headY + 8, 28, 8, Color(120, 40, 60));
        // Teeth
        for (int i = 0; i < 5; i++) {
            fillRect(cx - 12 + i * 6, headY + 6, 4, 5, Color(255, 255, 230));
        }
    }
    // Eyes
    fillCircle(cx - 6, headY - 6, 4, Color(255, 255, 255));
    fillCircle(cx - 5, headY - 6, 2, Color(30, 30, 30));
    fillCircle(cx + 6, headY - 6, 4, Color(255, 255, 255));
    fillCircle(cx + 7, headY - 6, 2, Color(30, 30, 30));
}

void Renderer::drawJalapeno(float cx, float cy, float timer) {
    float glow = std::sin(timer * 10.0f) * 0.3f + 0.7f;
    Color bodyC(static_cast<uint8_t>(200 * glow), static_cast<uint8_t>(50 * glow), 20);
    // Body - elongated
    fillRect(cx - 8, cy - 20, 16, 40, bodyC);
    fillCircle(cx, cy - 20, 8, bodyC);
    fillCircle(cx, cy + 20, 8, bodyC);
    // Eyes
    fillCircle(cx - 3, cy - 5, 3, Color(255, 255, 255));
    fillCircle(cx - 2, cy - 5, 1.5f, Color(30, 30, 30));
    fillCircle(cx + 3, cy - 5, 3, Color(255, 255, 255));
    fillCircle(cx + 4, cy - 5, 1.5f, Color(30, 30, 30));
    // Flame on top
    fillCircle(cx, cy - 28, 5, Color(255, 200, 50));
    fillCircle(cx, cy - 33, 3, Color(255, 255, 100));
}

void Renderer::drawTorchwood(float cx, float cy, int frame) {
    // Trunk
    fillRect(cx - 10, cy - 5, 20, 35, Color(100, 60, 20));
    fillRect(cx - 6, cy - 5, 12, 35, Color(120, 70, 25));
    // Eyes
    fillCircle(cx - 5, cy + 5, 3, Color(255, 200, 50));
    fillCircle(cx + 5, cy + 5, 3, Color(255, 200, 50));
    // Flame on top
    float flicker = (frame % 2 == 0) ? 0.0f : 3.0f;
    fillCircle(cx, cy - 12 - flicker, 12, Color(255, 150, 30, 200));
    fillCircle(cx, cy - 18 - flicker, 8, Color(255, 220, 80, 180));
    fillCircle(cx, cy - 22 - flicker, 5, Color(255, 255, 150, 150));
}

void Renderer::drawPlant(PlantType type, float cx, float cy, int frame, float extra) {
    switch (type) {
        case PlantType::Sunflower:  drawSunflower(cx, cy, frame); break;
        case PlantType::Peashooter: drawPeashooter(cx, cy, frame); break;
        case PlantType::WallNut:    drawWallNut(cx, cy, extra); break;
        case PlantType::SnowPea:    drawSnowPea(cx, cy, frame); break;
        case PlantType::CherryBomb: drawCherryBomb(cx, cy, extra); break;
        case PlantType::PotatoMine: drawPotatoMine(cx, cy, extra > 0.5f); break;
        case PlantType::Repeater:   drawRepeater(cx, cy, frame); break;
        case PlantType::Chomper:    drawChomper(cx, cy, extra < 0.5f); break;
        case PlantType::Jalapeno:   drawJalapeno(cx, cy, extra); break;
        case PlantType::Torchwood:  drawTorchwood(cx, cy, frame); break;
        default: break;
    }
}

// ============================================================================
// Zombies
// ============================================================================

static void drawZombieBody(float cx, float cy, int frame, Color shirtC, Color pantsC) {
    float walk = (frame % 2 == 0) ? -2.0f : 2.0f;
    // Legs
    fillRect(cx - 6, cy + 12, 6, 18 + walk, pantsC);
    fillRect(cx + 2, cy + 12, 6, 18 - walk, pantsC);
    // Shoes
    fillRect(cx - 8, cy + 28 + walk, 10, 5, Color(40, 40, 40));
    fillRect(cx, cy + 28 - walk, 10, 5, Color(40, 40, 40));
    // Body
    fillRect(cx - 8, cy - 5, 18, 20, shirtC);
    // Arms
    float armAngle = walk;
    fillRect(cx - 14, cy - 2 + armAngle, 8, 5, Color(140, 170, 120));
    fillRect(cx + 8, cy - 2 - armAngle, 8, 5, Color(140, 170, 120));
    // Hands (reaching forward)
    fillRect(cx + 14, cy - 4 - armAngle, 6, 4, Color(140, 170, 120));
}

static void drawZombieHead(float cx, float cy) {
    // Head
    fillCircle(cx, cy - 16, 12, Color(150, 180, 130));
    // Eyes
    fillCircle(cx - 4, cy - 19, 3, Color(255, 255, 200));
    fillCircle(cx - 3, cy - 19, 1.5f, Color(180, 30, 30));
    fillCircle(cx + 4, cy - 19, 3, Color(255, 255, 200));
    fillCircle(cx + 5, cy - 19, 1.5f, Color(180, 30, 30));
    // Mouth
    fillRect(cx - 4, cy - 10, 8, 3, Color(80, 40, 40));
}

void Renderer::drawBasicZombie(float cx, float cy, int frame, float hpPct) {
    Color shirt = (hpPct > 0.5f) ? Color(120, 100, 80) : Color(100, 80, 60);
    drawZombieBody(cx, cy, frame, shirt, Color(80, 80, 100));
    drawZombieHead(cx, cy);
}

void Renderer::drawConeZombie(float cx, float cy, int frame, float armorPct) {
    drawBasicZombie(cx, cy, frame, 1.0f);
    if (armorPct > 0) {
        // Cone
        Color coneC = (armorPct > 0.5f) ? Color(220, 140, 40) : Color(180, 100, 30);
        Graphics::drawLine(cx, cy - 38, cx - 10, cy - 18, coneC, 3);
        Graphics::drawLine(cx, cy - 38, cx + 10, cy - 18, coneC, 3);
        fillRect(cx - 10, cy - 20, 20, 4, coneC);
    }
}

void Renderer::drawBucketZombie(float cx, float cy, int frame, float armorPct) {
    drawBasicZombie(cx, cy, frame, 1.0f);
    if (armorPct > 0) {
        Color bucketC = (armorPct > 0.5f) ? Color(160, 160, 170) : Color(120, 120, 130);
        fillRect(cx - 11, cy - 30, 22, 16, bucketC);
        outlineRect(cx - 11, cy - 30, 22, 16, Color(100, 100, 110));
    }
}

void Renderer::drawFlagZombie(float cx, float cy, int frame) {
    drawBasicZombie(cx, cy, frame, 1.0f);
    // Flag
    Graphics::drawLine(cx + 6, cy - 28, cx + 6, cy, Color(80, 60, 40), 2);
    fillRect(cx + 6, cy - 28, 18, 12, Color(200, 50, 50));
}

void Renderer::drawNewspaperZombie(float cx, float cy, int frame, bool angry) {
    Color shirt = angry ? Color(160, 60, 60) : Color(120, 100, 80);
    drawZombieBody(cx, cy, frame, shirt, Color(80, 80, 100));
    drawZombieHead(cx, cy);
    if (!angry) {
        // Newspaper
        fillRect(cx - 18, cy - 8, 14, 20, Color(220, 210, 180));
        outlineRect(cx - 18, cy - 8, 14, 20, Color(160, 150, 120));
    }
}

void Renderer::drawPoleVaultZombie(float cx, float cy, int frame, bool jumped) {
    drawZombieBody(cx, cy, frame, Color(100, 100, 140), Color(60, 60, 80));
    drawZombieHead(cx, cy);
    if (!jumped) {
        // Pole
        Graphics::drawLine(cx + 10, cy - 30, cx + 10, cy + 20, Color(140, 100, 40), 3);
    }
}

void Renderer::drawFootballZombie(float cx, float cy, int frame, float hpPct) {
    Color jersey = (hpPct > 0.5f) ? Color(40, 80, 40) : Color(30, 60, 30);
    drawZombieBody(cx, cy, frame, jersey, Color(240, 240, 240));
    // Helmet
    fillCircle(cx, cy - 16, 14, Color(40, 80, 40));
    fillRect(cx - 2, cy - 30, 4, 8, Color(40, 80, 40));  // top guard
    // Face mask
    Graphics::drawLine(cx - 6, cy - 14, cx + 6, cy - 14, Color(180, 180, 180), 2);
    // Eyes behind mask
    fillCircle(cx - 3, cy - 18, 2, Color(255, 50, 50));
    fillCircle(cx + 3, cy - 18, 2, Color(255, 50, 50));
    // Shoulder pads
    fillRect(cx - 16, cy - 6, 8, 6, Color(60, 100, 60));
    fillRect(cx + 10, cy - 6, 8, 6, Color(60, 100, 60));
}

void Renderer::drawDancingZombie(float cx, float cy, int frame) {
    float sway = std::sin(frame * 1.5f) * 4.0f;
    drawZombieBody(cx + sway, cy, frame, Color(200, 200, 220), Color(30, 30, 30));
    drawZombieHead(cx + sway, cy);
    // Disco hair
    fillCircle(cx + sway - 6, cy - 28, 4, Color(50, 50, 50));
    fillCircle(cx + sway + 0, cy - 30, 4, Color(50, 50, 50));
    fillCircle(cx + sway + 6, cy - 28, 4, Color(50, 50, 50));
}

void Renderer::drawImpZombie(float cx, float cy, int frame, float hpPct) {
    float walk = (frame % 2 == 0) ? -1.5f : 1.5f;
    // Smaller zombie - scaled down ~60%
    // Legs
    fillRect(cx - 4, cy + 6, 4, 12 + walk, Color(70, 70, 90));
    fillRect(cx + 1, cy + 6, 4, 12 - walk, Color(70, 70, 90));
    // Shoes
    fillRect(cx - 5, cy + 17 + walk, 7, 3, Color(40, 40, 40));
    fillRect(cx, cy + 17 - walk, 7, 3, Color(40, 40, 40));
    // Body
    fillRect(cx - 6, cy - 4, 13, 12, Color(100, 80, 60));
    // Arms
    fillRect(cx - 10, cy - 2 + walk, 6, 4, Color(140, 170, 120));
    fillRect(cx + 6, cy - 2 - walk, 6, 4, Color(140, 170, 120));
    // Head (smaller)
    fillCircle(cx, cy - 12, 8, Color(150, 180, 130));
    // Little angry eyes
    fillCircle(cx - 3, cy - 14, 2, Color(255, 255, 200));
    fillCircle(cx - 2, cy - 14, 1, Color(200, 30, 30));
    fillCircle(cx + 3, cy - 14, 2, Color(255, 255, 200));
    fillCircle(cx + 4, cy - 14, 1, Color(200, 30, 30));
    // Mouth (grin)
    fillRect(cx - 3, cy - 8, 6, 2, Color(80, 40, 40));
}

void Renderer::drawGargantuarZombie(float cx, float cy, int frame, float hpPct) {
    float walk = (frame % 2 == 0) ? -3.0f : 3.0f;
    // Much larger zombie - scaled up ~1.5x
    float scale = 1.5f;
    Color skinC = (hpPct > 0.5f) ? Color(130, 160, 110) : Color(110, 130, 90);

    // Legs (thick)
    fillRect(cx - 10, cy + 8, 10, 24 + walk, Color(60, 60, 80));
    fillRect(cx + 2, cy + 8, 10, 24 - walk, Color(60, 60, 80));
    // Big boots
    fillRect(cx - 14, cy + 30 + walk, 16, 8, Color(50, 40, 30));
    fillRect(cx, cy + 30 - walk, 16, 8, Color(50, 40, 30));

    // Massive body
    fillRect(cx - 16, cy - 14, 34, 26, Color(80, 60, 40));
    // Belt
    fillRect(cx - 16, cy + 6, 34, 5, Color(50, 40, 30));

    // Arms (thick)
    fillRect(cx - 24, cy - 10 + walk, 12, 8, skinC);
    fillRect(cx + 14, cy - 10 - walk, 12, 8, skinC);
    // Fists
    fillCircle(cx - 26, cy - 6 + walk, 6, skinC);
    fillCircle(cx + 28, cy - 6 - walk, 6, skinC);

    // Head (big)
    fillCircle(cx, cy - 24, 16, skinC);
    // Heavy brow
    fillRect(cx - 14, cy - 30, 28, 5, Color(100, 120, 80));
    // Red angry eyes
    fillCircle(cx - 6, cy - 28, 4, Color(255, 255, 180));
    fillCircle(cx - 5, cy - 28, 2.5f, Color(220, 40, 40));
    fillCircle(cx + 6, cy - 28, 4, Color(255, 255, 180));
    fillCircle(cx + 7, cy - 28, 2.5f, Color(220, 40, 40));
    // Mouth (snarl)
    fillRect(cx - 6, cy - 18, 12, 4, Color(100, 40, 40));
    // Teeth
    for (int i = 0; i < 4; i++) {
        fillRect(cx - 5 + i * 3, cy - 19, 2, 3, Color(230, 230, 200));
    }

    // Telephone pole weapon
    Graphics::drawLine(cx + 16, cy - 40, cx + 20, cy + 10, Color(120, 80, 30), 4);
    // Cross bar on pole
    fillRect(cx + 12, cy - 38, 14, 3, Color(100, 70, 25));

    // HP bar (boss indicator)
    float barW = 40.0f;
    float barX = cx - barW * 0.5f;
    float barY = cy - 46;
    fillRect(barX, barY, barW, 4, Color(60, 60, 60));
    fillRect(barX, barY, barW * hpPct, 4, Color(220, 40, 40));
    outlineRect(barX, barY, barW, 4, Color(30, 30, 30));
}

void Renderer::drawZombie(ZombieType type, float cx, float cy, int frame,
                           float hpPct, float armorPct, bool special) {
    switch (type) {
        case ZombieType::Basic:      drawBasicZombie(cx, cy, frame, hpPct); break;
        case ZombieType::Cone:       drawConeZombie(cx, cy, frame, armorPct); break;
        case ZombieType::Bucket:     drawBucketZombie(cx, cy, frame, armorPct); break;
        case ZombieType::Flag:       drawFlagZombie(cx, cy, frame); break;
        case ZombieType::Newspaper:  drawNewspaperZombie(cx, cy, frame, special); break;
        case ZombieType::PoleVault:  drawPoleVaultZombie(cx, cy, frame, special); break;
        case ZombieType::Football:   drawFootballZombie(cx, cy, frame, hpPct); break;
        case ZombieType::Dancing:    drawDancingZombie(cx, cy, frame); break;
        case ZombieType::Imp:        drawImpZombie(cx, cy, frame, hpPct); break;
        case ZombieType::Gargantuar: drawGargantuarZombie(cx, cy, frame, hpPct); break;
        default: break;
    }
}

// ============================================================================
// Projectiles
// ============================================================================

void Renderer::drawPea(float x, float y) {
    fillCircle(x, y, 6, Color(100, 200, 50));
    fillCircle(x - 1, y - 1, 3, Color(150, 230, 100));
}

void Renderer::drawSnowPeaBullet(float x, float y) {
    fillCircle(x, y, 6, Color(100, 180, 220));
    fillCircle(x - 1, y - 1, 3, Color(180, 220, 255));
}

void Renderer::drawFirePea(float x, float y) {
    fillCircle(x, y, 8, Color(255, 150, 30));
    fillCircle(x, y, 5, Color(255, 220, 80));
    fillCircle(x - 1, y - 1, 3, Color(255, 255, 200));
}

// ============================================================================
// Sun
// ============================================================================

void Renderer::drawSun(float cx, float cy, float scale) {
    float r = 14.0f * scale;
    // Rays
    Color rayC(255, 230, 80, 150);
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f / 4.0f;
        float rx = cx + std::cos(angle) * (r + 6);
        float ry = cy + std::sin(angle) * (r + 6);
        Graphics::drawLine(cx, cy, rx, ry, rayC, 2);
    }
    // Body
    fillCircle(cx, cy, r, Color(255, 220, 50));
    fillCircle(cx - 2, cy - 2, r * 0.6f, Color(255, 240, 100));
    // Eyes
    float es = scale;
    fillCircle(cx - 3 * es, cy - 3 * es, 2 * es, Color(60, 60, 60));
    fillCircle(cx + 3 * es, cy - 3 * es, 2 * es, Color(60, 60, 60));
    // Smile
    fillRect(cx - 3 * es, cy + 2 * es, 6 * es, 1.5f * es, Color(60, 60, 60));
}

// ============================================================================
// Grid / Environment
// ============================================================================

void Renderer::drawGrassCell(int row, int col, bool highlight) {
    float x = GRID_OFFSET_X + col * CELL_W;
    float y = GRID_OFFSET_Y + row * CELL_H;
    // Checkerboard grass
    bool light = (row + col) % 2 == 0;
    Color grass = light ? Color(100, 180, 60) : Color(80, 160, 50);
    if (highlight) {
        grass = Color(grass.r + 30, grass.g + 30, grass.b + 10, 255);
    }
    fillRect(x, y, CELL_W, CELL_H, grass);

    // Grass blade detail (deterministic pattern based on row/col)
    unsigned int seed = static_cast<unsigned int>(row * 37 + col * 71);
    Color bladeC = light ? Color(90, 170, 55) : Color(70, 150, 45);
    Color bladeLight = light ? Color(115, 195, 70) : Color(95, 175, 60);
    for (int i = 0; i < 6; i++) {
        seed = seed * 1103515245u + 12345u;
        float bx = x + 8 + (seed % static_cast<unsigned int>(CELL_W - 16));
        seed = seed * 1103515245u + 12345u;
        float by = y + 10 + (seed % static_cast<unsigned int>(CELL_H - 20));
        seed = seed * 1103515245u + 12345u;
        float bh = 4.0f + (seed % 8);
        Graphics::drawLine(bx, by, bx - 1, by - bh, (i % 2 == 0) ? bladeC : bladeLight, 1);
    }

    // Small flowers on some cells (deterministic)
    seed = static_cast<unsigned int>(row * 53 + col * 97 + 7);
    if (seed % 5 == 0) {
        float fx = x + 15 + (seed % static_cast<unsigned int>(CELL_W - 30));
        float fy = y + 20 + ((seed / 5) % static_cast<unsigned int>(CELL_H - 40));
        Color flowerC = (seed % 3 == 0) ? Color(255, 255, 150) :
                        (seed % 3 == 1) ? Color(255, 200, 200) : Color(200, 200, 255);
        fillCircle(fx, fy, 2.5f, flowerC);
        fillCircle(fx, fy, 1.2f, Color(255, 220, 80));
    }

    outlineRect(x, y, CELL_W, CELL_H, Color(70, 130, 40, 40));
}

void Renderer::drawLawnmower(float x, float y, bool triggered) {
    Color bodyC = triggered ? Color(200, 50, 50) : Color(180, 180, 180);
    fillRect(x - 15, y - 8, 30, 16, bodyC);
    fillRect(x + 10, y - 12, 8, 8, Color(60, 60, 60)); // exhaust
    // Wheels
    fillCircle(x - 10, y + 10, 5, Color(40, 40, 40));
    fillCircle(x + 10, y + 10, 5, Color(40, 40, 40));
}

// ============================================================================
// Effects
// ============================================================================

void Renderer::drawExplosion(float cx, float cy, float radius, float alpha) {
    uint8_t a = static_cast<uint8_t>(alpha * 255);
    fillCircle(cx, cy, radius, Color(255, 200, 50, a));
    fillCircle(cx, cy, radius * 0.7f, Color(255, 150, 30, a));
    fillCircle(cx, cy, radius * 0.4f, Color(255, 255, 200, a));
}

void Renderer::drawFireLane(int row, float progress) {
    float y = GRID_OFFSET_Y + row * CELL_H;
    float maxX = GRID_OFFSET_X + GRID_COLS * CELL_W;
    float fireX = GRID_OFFSET_X + progress * (maxX - GRID_OFFSET_X);

    for (float x = GRID_OFFSET_X; x < fireX; x += 20.0f) {
        float flicker = std::sin(x * 0.1f + progress * 20.0f) * 10.0f;
        fillCircle(x, y + CELL_H * 0.5f + flicker, 15, Color(255, 100, 20, 180));
        fillCircle(x, y + CELL_H * 0.5f + flicker - 8, 10, Color(255, 200, 50, 150));
    }
}

// ============================================================================
// HUD Helpers
// ============================================================================

void Renderer::drawSeedPacket(PlantType type, float x, float y, float w, float h,
                               bool selected, bool affordable, float cooldownPct) {
    // Background
    Color bg = selected ? Color(200, 220, 180) : (affordable ? Color(180, 160, 120) : Color(100, 100, 100));
    fillRect(x, y, w, h, bg);
    outlineRect(x, y, w, h, selected ? Color(255, 255, 100) : Color(80, 60, 40));

    // Mini plant preview
    drawPlant(type, x + w * 0.5f, y + h * 0.45f, 0, 1.0f);

    // Cost text area
    auto& data = getPlantData(type);
    fillRect(x, y + h - 14, w, 14, Color(60, 40, 20, 180));

    // Cooldown overlay
    if (cooldownPct > 0.0f) {
        float cdH = h * cooldownPct;
        fillRect(x, y, w, cdH, Color(0, 0, 0, 120));
    }
}

void Renderer::drawSunIcon(float x, float y, float size) {
    drawSun(x, y, size / 28.0f);
}

void Renderer::drawShovel(float x, float y, float size, bool selected) {
    Color bg = selected ? Color(200, 180, 140) : Color(160, 140, 100);
    fillRect(x - size * 0.5f, y - size * 0.5f, size, size, bg);
    outlineRect(x - size * 0.5f, y - size * 0.5f, size, size,
                selected ? Color(255, 255, 100) : Color(100, 80, 50));
    // Shovel blade
    fillRect(x - 6, y - size * 0.3f, 12, size * 0.3f, Color(160, 160, 170));
    // Handle
    fillRect(x - 2, y, 4, size * 0.35f, Color(120, 80, 40));
}

// ============================================================================
// Drops
// ============================================================================

void Renderer::drawDrop(DropType type, float cx, float cy, float scale) {
    float r = 10.0f * scale;
    switch (type) {
        case DropType::Coin: {
            // Gold coin
            fillCircle(cx, cy, r, Color(255, 210, 50));
            outlineCircle(cx, cy, r, Color(200, 160, 30));
            fillCircle(cx, cy, r * 0.6f, Color(255, 230, 100));
            // $ symbol
            fillRect(cx - 1, cy - r * 0.4f, 2, r * 0.8f, Color(200, 160, 30));
            break;
        }
        case DropType::Diamond: {
            // Blue diamond
            Color diaC(100, 180, 255);
            // Diamond shape using triangles approximation
            fillRect(cx - r * 0.6f, cy - 2, r * 1.2f, 4, diaC);
            fillRect(cx - r * 0.3f, cy - r * 0.6f, r * 0.6f, r * 1.2f, diaC);
            fillCircle(cx, cy, r * 0.5f, Color(150, 210, 255));
            // Sparkle
            fillCircle(cx - 2, cy - 3, 2, Color(220, 240, 255));
            break;
        }
        case DropType::PlantBoost: {
            // Green arrow up
            fillCircle(cx, cy, r, Color(60, 180, 60));
            outlineCircle(cx, cy, r, Color(40, 140, 40));
            // Up arrow
            Graphics::drawLine(cx, cy - r * 0.5f, cx, cy + r * 0.4f, Color(255, 255, 255), 2);
            Graphics::drawLine(cx - 4, cy - r * 0.2f, cx, cy - r * 0.5f, Color(255, 255, 255), 2);
            Graphics::drawLine(cx + 4, cy - r * 0.2f, cx, cy - r * 0.5f, Color(255, 255, 255), 2);
            break;
        }
        case DropType::SunBurst: {
            // Orange sun burst
            fillCircle(cx, cy, r, Color(255, 180, 40));
            // Rays
            for (int i = 0; i < 6; i++) {
                float angle = i * 3.14159f / 3.0f;
                float rx = cx + std::cos(angle) * (r + 3);
                float ry = cy + std::sin(angle) * (r + 3);
                Graphics::drawLine(cx, cy, rx, ry, Color(255, 220, 80), 2);
            }
            fillCircle(cx, cy, r * 0.6f, Color(255, 240, 120));
            break;
        }
        case DropType::FreezeAll: {
            // Blue ice crystal
            fillCircle(cx, cy, r, Color(80, 160, 220));
            outlineCircle(cx, cy, r, Color(60, 120, 180));
            // Snowflake pattern
            for (int i = 0; i < 3; i++) {
                float angle = i * 3.14159f / 3.0f;
                float x1 = cx + std::cos(angle) * r * 0.6f;
                float y1 = cy + std::sin(angle) * r * 0.6f;
                float x2 = cx - std::cos(angle) * r * 0.6f;
                float y2 = cy - std::sin(angle) * r * 0.6f;
                Graphics::drawLine(x1, y1, x2, y2, Color(200, 230, 255), 2);
            }
            fillCircle(cx, cy, 3, Color(220, 240, 255));
            break;
        }
    }
}

} // namespace PVZ
