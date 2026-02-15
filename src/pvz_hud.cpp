#include "pvz_hud.hpp"
#include "pvz_renderer.hpp"
#include "pvz_font.hpp"
#include <cstdio>

namespace PVZ {

void HUD::init(GameState* gs, const std::vector<PlantType>& available) {
    m_gs = gs;
    m_availablePlants = available;
    m_shovelX = PACKET_START_X + m_availablePlants.size() * (PACKET_W + PACKET_GAP) + 10;
}

PlantType HUD::getSelectedPlantType() const {
    if (m_gs->selectedPlant < 0 || m_gs->selectedPlant >= static_cast<int>(m_availablePlants.size()))
        return PlantType::Peashooter;
    return m_availablePlants[m_gs->selectedPlant];
}

void HUD::clearSelection() {
    m_gs->selectedPlant = -1;
    m_gs->shovelActive = false;
}

void HUD::handleInput(float mouseX, float mouseY, bool clicked) {
    if (!clicked) return;
    if (mouseY > BAR_H) return;

    // Check seed packets
    for (int i = 0; i < static_cast<int>(m_availablePlants.size()); i++) {
        float px = PACKET_START_X + i * (PACKET_W + PACKET_GAP);
        if (mouseX >= px && mouseX <= px + PACKET_W && mouseY >= BAR_Y && mouseY <= BAR_Y + PACKET_H) {
            auto& data = getPlantData(m_availablePlants[i]);
            if (m_gs->sunCount >= data.cost && m_gs->seedCooldowns[static_cast<int>(m_availablePlants[i])] <= 0) {
                m_gs->selectedPlant = (m_gs->selectedPlant == i) ? -1 : i;
                m_gs->shovelActive = false;
            }
            return;
        }
    }

    // Check shovel
    if (mouseX >= m_shovelX && mouseX <= m_shovelX + 40 && mouseY >= 10 && mouseY <= 50) {
        m_gs->shovelActive = !m_gs->shovelActive;
        m_gs->selectedPlant = -1;
        return;
    }
}

void HUD::update(float dt) {
    // Update cooldowns
    for (int i = 0; i < PLANT_COUNT; i++) {
        if (m_gs->seedCooldowns[i] > 0) {
            m_gs->seedCooldowns[i] -= dt;
        }
    }
}

void HUD::render() {
    // Bar background
    SF3::Rect bar{0, BAR_Y, SCREEN_W, BAR_H};
    SF3::Graphics::drawRect(bar, SF3::Color(50, 30, 10, 220));

    renderSunCounter();
    renderSeedPackets();
    renderShovel();
    renderWaveInfo();
}

void HUD::renderSunCounter() {
    Renderer::drawSunIcon(SUN_DISPLAY_X + 20, SUN_DISPLAY_Y + 20, 28);

    // Sun count with font
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%d", m_gs->sunCount);
    auto& fm = FontManager::instance();
    fm.drawTextShadow(buf, SUN_DISPLAY_X + 42, SUN_DISPLAY_Y + 12, SF3::Color(255, 220, 50));
}

void HUD::renderSeedPackets() {
    auto& fm = FontManager::instance();

    for (int i = 0; i < static_cast<int>(m_availablePlants.size()); i++) {
        float px = PACKET_START_X + i * (PACKET_W + PACKET_GAP);
        auto type = m_availablePlants[i];
        auto& data = getPlantData(type);
        bool selected = (m_gs->selectedPlant == i);
        bool affordable = (m_gs->sunCount >= data.cost);
        float cooldown = m_gs->seedCooldowns[static_cast<int>(type)];
        float maxCd = data.cooldown;
        float cdPct = (cooldown > 0 && maxCd > 0) ? (cooldown / maxCd) : 0.0f;

        Renderer::drawSeedPacket(type, px, BAR_Y + 2, PACKET_W, PACKET_H, selected, affordable, cdPct);

        // Draw cost number below packet mini-plant
        char costBuf[8];
        std::snprintf(costBuf, sizeof(costBuf), "%d", data.cost);
        SF3::Color numC = affordable ? SF3::Color(255, 230, 100) : SF3::Color(150, 150, 150);
        fm.drawSmallCentered(costBuf, px + PACKET_W * 0.5f, BAR_Y + PACKET_H - 14, numC);
    }
}

void HUD::renderShovel() {
    Renderer::drawShovel(m_shovelX + 20, 30, 40, m_gs->shovelActive);
}

void HUD::renderWaveInfo() {
    if (!m_gs->levelDef) return;
    auto& fm = FontManager::instance();

    float infoX = SCREEN_W - 120;
    float infoY = 10;

    // Wave label
    char waveBuf[32];
    int totalWaves = static_cast<int>(m_gs->levelDef->waves.size());
    std::snprintf(waveBuf, sizeof(waveBuf), u8"波次 %d/%d", m_gs->currentWave, totalWaves);
    fm.drawSmall(waveBuf, infoX, infoY - 2, SF3::Color(200, 180, 150));

    // Wave progress bar
    float progress = (totalWaves > 0) ? static_cast<float>(m_gs->currentWave) / totalWaves : 0.0f;
    float barY = infoY + 16;

    SF3::Graphics::drawRect({infoX, barY, 100, 10}, SF3::Color(40, 20, 10));
    SF3::Graphics::drawRect({infoX, barY, 100 * progress, 10}, SF3::Color(200, 50, 50));
    SF3::Graphics::drawRectOutline({infoX, barY, 100, 10}, SF3::Color(100, 60, 30));

    // Wave markers
    for (int i = 0; i < totalWaves; i++) {
        float mx = infoX + (100.0f * i / totalWaves);
        SF3::Graphics::drawLine(mx, barY, mx, barY + 10, SF3::Color(80, 50, 20), 1);
    }

    // Flag icon at end
    SF3::Graphics::drawRect({infoX + 95, barY - 4, 8, 6}, SF3::Color(200, 50, 50));

    // Zombie count
    float countY = barY + 16;
    char zombieBuf[32];
    std::snprintf(zombieBuf, sizeof(zombieBuf), u8"殭屍: %d", m_gs->totalZombiesAlive);
    fm.drawSmall(zombieBuf, infoX, countY, SF3::Color(150, 180, 130));
}

} // namespace PVZ
