#pragma once

#include <sf3.hpp>
#include "pvz_data.hpp"
#include "pvz_systems.hpp"

namespace PVZ {

class HUD {
public:
    void init(GameState* gs, const std::vector<PlantType>& available);
    void handleInput(float mouseX, float mouseY, bool clicked);
    void update(float dt);
    void render();

    int getSelectedPlantIndex() const { return m_gs->selectedPlant; }
    PlantType getSelectedPlantType() const;
    bool isShovelActive() const { return m_gs->shovelActive; }
    void clearSelection();

    static constexpr float BAR_H = 70.0f;

private:
    GameState* m_gs = nullptr;
    std::vector<PlantType> m_availablePlants;

    // Layout
    static constexpr float BAR_Y = 0.0f;
    static constexpr float PACKET_W = 55.0f;
    static constexpr float PACKET_H = 65.0f;
    static constexpr float PACKET_START_X = 100.0f;
    static constexpr float PACKET_GAP = 4.0f;
    static constexpr float SUN_DISPLAY_X = 20.0f;
    static constexpr float SUN_DISPLAY_Y = 10.0f;

    float m_shovelX = 0.0f;

    void renderSunCounter();
    void renderSeedPackets();
    void renderWaveInfo();
    void renderShovel();
};

} // namespace PVZ
