#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstdint>

namespace PVZ {

// ============================================================================
// Plant Types
// ============================================================================

enum class PlantType : uint8_t {
    Sunflower = 0,
    Peashooter,
    WallNut,
    SnowPea,
    CherryBomb,
    PotatoMine,
    Repeater,
    Chomper,
    Jalapeno,
    Torchwood,
    COUNT
};

struct PlantData {
    PlantType type;
    const char* name;
    const char* nameZh;      // Traditional Chinese name
    int cost;
    int hp;
    float cooldown;           // seed packet recharge (seconds)
    float attackRate;          // seconds between attacks (0 = no attack)
    int damage;                // per projectile
};

constexpr int PLANT_COUNT = static_cast<int>(PlantType::COUNT);

inline const PlantData PLANTS[PLANT_COUNT] = {
    //                                        cost  HP   cd    atkRate dmg
    { PlantType::Sunflower,  "Sunflower",  u8"向日葵",    50,  100, 7.5f,  0.0f,  0 },
    { PlantType::Peashooter, "Peashooter", u8"豌豆射手",  100, 100, 7.5f,  1.4f, 20 },
    { PlantType::WallNut,    "Wall-nut",   u8"堅果牆",    50,  800, 25.0f, 0.0f,  0 },
    { PlantType::SnowPea,    "Snow Pea",   u8"寒冰射手",  175, 100, 7.5f,  1.4f, 20 },
    { PlantType::CherryBomb, "Cherry Bomb",u8"櫻桃炸彈",  150,   1, 45.0f, 0.0f,  0 },
    { PlantType::PotatoMine, "Potato Mine",u8"土豆地雷",   25, 100, 25.0f, 0.0f,  0 },
    { PlantType::Repeater,   "Repeater",   u8"雙發射手",  200, 100, 7.5f,  1.4f, 20 },
    { PlantType::Chomper,    "Chomper",    u8"大嘴花",    150, 100, 7.5f,  0.0f,  0 },
    { PlantType::Jalapeno,   "Jalapeno",   u8"火爆辣椒",  125,   1, 45.0f, 0.0f,  0 },
    { PlantType::Torchwood,  "Torchwood",  u8"火炬樹樁",  175, 200, 7.5f,  0.0f,  0 },
};

inline const PlantData& getPlantData(PlantType type) {
    return PLANTS[static_cast<int>(type)];
}

// ============================================================================
// Zombie Types
// ============================================================================

enum class ZombieType : uint8_t {
    Basic = 0,
    Cone,
    Bucket,
    Flag,
    Newspaper,
    PoleVault,
    Football,
    Dancing,
    Imp,          // Thrown by Gargantuar
    Gargantuar,   // Boss zombie
    COUNT
};

struct ZombieData {
    ZombieType type;
    const char* name;
    const char* nameZh;
    int hp;
    int armorHp;         // extra HP layer
    float speed;
    float speedAlt;
    int coinValue;
    int biteDamage;
    float eatRate;
    bool isBoss;         // boss zombies are drawn larger
};

constexpr int ZOMBIE_COUNT = static_cast<int>(ZombieType::COUNT);

inline const ZombieData ZOMBIES[ZOMBIE_COUNT] = {
    //                                              HP   armor spd   altSpd coin bite eatR  boss
    { ZombieType::Basic,     "Zombie",      u8"殭屍",       100,   0, 0.25f, 0.25f, 1,  10, 0.5f, false },
    { ZombieType::Cone,      "Conehead",    u8"路障殭屍",   100, 200, 0.25f, 0.25f, 2,  10, 0.5f, false },
    { ZombieType::Bucket,    "Buckethead",  u8"鐵桶殭屍",   100, 500, 0.22f, 0.22f, 4,  10, 0.5f, false },
    { ZombieType::Flag,      "Flag Zombie", u8"旗幟殭屍",   100,   0, 0.35f, 0.35f, 1,  10, 0.5f, false },
    { ZombieType::Newspaper, "Newspaper",   u8"讀報殭屍",   100, 100, 0.25f, 0.65f, 2,  15, 0.4f, false },
    { ZombieType::PoleVault, "Pole Vaulter",u8"撐竿殭屍",   175,   0, 0.70f, 0.20f, 3,  10, 0.5f, false },
    { ZombieType::Football,  "Football",    u8"橄欖球殭屍", 300, 100, 0.55f, 0.55f, 5,  20, 0.4f, false },
    { ZombieType::Dancing,   "Dancing",     u8"舞王殭屍",   200,   0, 0.30f, 0.30f, 4,  10, 0.5f, false },
    { ZombieType::Imp,       "Imp",         u8"小鬼殭屍",    80,   0, 0.50f, 0.50f, 1,   8, 0.3f, false },
    { ZombieType::Gargantuar,"Gargantuar",  u8"巨人殭屍",  1500, 300, 0.12f, 0.12f, 20, 80, 1.0f, true  },
};

inline const ZombieData& getZombieData(ZombieType type) {
    return ZOMBIES[static_cast<int>(type)];
}

// ============================================================================
// Projectile Types
// ============================================================================

enum class ProjectileType : uint8_t {
    Pea = 0,
    SnowPea,
    FirePea,    // pea that passed through Torchwood
};

// ============================================================================
// Drop / Power-Up Types
// ============================================================================

enum class DropType : uint8_t {
    Coin = 0,        // +10 coins
    Diamond,         // +50 coins
    PlantBoost,      // All plants attack 2x faster for 10s
    SunBurst,        // +100 sun
    FreezeAll,       // Freeze all zombies for 5s
};

// ============================================================================
// Game Grid Constants
// ============================================================================

constexpr int GRID_ROWS = 5;
constexpr int GRID_COLS = 9;
constexpr float CELL_W = 80.0f;
constexpr float CELL_H = 100.0f;
constexpr float GRID_OFFSET_X = 100.0f;
constexpr float GRID_OFFSET_Y = 80.0f;
constexpr float SCREEN_W = 900.0f;
constexpr float SCREEN_H = 600.0f;

inline float cellCenterX(int col) { return GRID_OFFSET_X + col * CELL_W + CELL_W * 0.5f; }
inline float cellCenterY(int row) { return GRID_OFFSET_Y + row * CELL_H + CELL_H * 0.5f; }
inline int   colFromX(float x)    { int c = static_cast<int>((x - GRID_OFFSET_X) / CELL_W); return (c < 0) ? -1 : (c >= GRID_COLS ? -1 : c); }
inline int   rowFromY(float y)    { int r = static_cast<int>((y - GRID_OFFSET_Y) / CELL_H); return (r < 0) ? -1 : (r >= GRID_ROWS ? -1 : r); }

// ============================================================================
// Wave / Level Definitions
// ============================================================================

struct WaveEntry {
    ZombieType type;
    int count;
    float delayBetween;
};

struct Wave {
    std::vector<WaveEntry> entries;
    float delayAfter;
    bool isFlagWave;
};

struct LevelDef {
    int levelNumber;
    const char* name;
    const char* nameZh;
    int startingSun;
    bool hasSkyDrop;
    bool isNight;        // dark overlay, limited visibility
    bool hasFog;         // right-side fog obscuring zombies
    std::vector<PlantType> availablePlants;
    std::vector<Wave> waves;
    int coinReward;
};

// Helpers
inline Wave W(std::vector<WaveEntry> entries, float delay, bool flag = false) {
    return Wave{std::move(entries), delay, flag};
}
inline WaveEntry Z(ZombieType t, int n, float d) {
    return WaveEntry{t, n, d};
}

inline std::vector<LevelDef> buildLevelDefs() {
    using P = PlantType;
    using Zt = ZombieType;
    std::vector<LevelDef> levels;
    LevelDef lv;

    // ===== Level 1: Tutorial =====
    lv = {1, "Day 1", u8"白天 1 - 初次相遇", 200, true, false, false,
        {P::Peashooter, P::Sunflower}, {}, 50};
    lv.waves = {
        W({Z(Zt::Basic,1,0)}, 30),
        W({Z(Zt::Basic,2,8)}, 25),
        W({Z(Zt::Basic,3,6)}, 20),
        W({Z(Zt::Basic,5,4)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 2: Economy =====
    lv = {2, "Day 2", u8"白天 2 - 陽光經濟", 150, true, false, false,
        {P::Peashooter, P::Sunflower}, {}, 50};
    lv.waves = {
        W({Z(Zt::Basic,2,8)}, 30),
        W({Z(Zt::Basic,3,6)}, 25),
        W({Z(Zt::Basic,3,5), Z(Zt::Cone,1,5)}, 20),
        W({Z(Zt::Basic,4,4), Z(Zt::Cone,2,4)}, 15),
        W({Z(Zt::Basic,6,3), Z(Zt::Cone,3,3)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 3: Defense =====
    lv = {3, "Day 3", u8"白天 3 - 堅果防線", 100, true, false, false,
        {P::Peashooter, P::Sunflower, P::WallNut}, {}, 75};
    lv.waves = {
        W({Z(Zt::Basic,3,6)}, 25),
        W({Z(Zt::Cone,2,5), Z(Zt::Basic,2,5)}, 20),
        W({Z(Zt::Cone,3,4), Z(Zt::Basic,3,4)}, 18),
        W({Z(Zt::Cone,4,3), Z(Zt::Basic,4,3)}, 15),
        W({Z(Zt::Bucket,1,0), Z(Zt::Cone,4,3), Z(Zt::Basic,5,3)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 4: Freeze & Boom =====
    lv = {4, "Day 4", u8"白天 4 - 冰與火", 75, true, false, false,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::CherryBomb}, {}, 75};
    lv.waves = {
        W({Z(Zt::Basic,4,5), Z(Zt::Cone,1,5)}, 25),
        W({Z(Zt::Cone,3,4), Z(Zt::Basic,3,4)}, 20),
        W({Z(Zt::Bucket,1,0), Z(Zt::Cone,3,4)}, 18),
        W({Z(Zt::Basic,5,3), Z(Zt::Cone,4,3), Z(Zt::Bucket,2,3)}, 15),
        W({Z(Zt::Bucket,3,2.5f), Z(Zt::Cone,5,2.5f), Z(Zt::Basic,5,2)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 5: Newspaper & Mine =====
    lv = {5, "Day 5", u8"白天 5 - 暴怒報紙", 75, true, false, false,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::CherryBomb, P::PotatoMine}, {}, 100};
    lv.waves = {
        W({Z(Zt::Newspaper,2,6), Z(Zt::Basic,2,6)}, 25),
        W({Z(Zt::Cone,3,4), Z(Zt::Newspaper,2,5)}, 22),
        W({Z(Zt::Newspaper,3,4), Z(Zt::Basic,4,4)}, 18),
        W({Z(Zt::Flag,1,0), Z(Zt::Newspaper,4,3), Z(Zt::Cone,3,3)}, 15),
        W({Z(Zt::Bucket,2,3), Z(Zt::Newspaper,4,2.5f), Z(Zt::Basic,6,2)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 6: Night - Pole Vaulter =====
    lv = {6, "Night 1", u8"夜晚 1 - 飛躍危機", 50, true, true, false,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::Repeater, P::PotatoMine}, {}, 100};
    lv.waves = {
        W({Z(Zt::Basic,3,6), Z(Zt::PoleVault,1,0)}, 25),
        W({Z(Zt::PoleVault,2,5), Z(Zt::Cone,2,5)}, 22),
        W({Z(Zt::PoleVault,3,4), Z(Zt::Basic,4,4)}, 18),
        W({Z(Zt::PoleVault,3,3), Z(Zt::Cone,3,3), Z(Zt::Bucket,1,3)}, 15),
        W({Z(Zt::Flag,1,0), Z(Zt::PoleVault,4,2.5f), Z(Zt::Bucket,2,3), Z(Zt::Cone,4,2.5f)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 7: Night - Football Blitz =====
    lv = {7, "Night 2", u8"夜晚 2 - 橄欖球衝鋒", 50, true, true, false,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::Repeater, P::CherryBomb, P::Chomper}, {}, 125};
    lv.waves = {
        W({Z(Zt::Basic,4,5), Z(Zt::Cone,2,5)}, 25),
        W({Z(Zt::Football,1,0), Z(Zt::Basic,3,5)}, 22),
        W({Z(Zt::Football,1,0), Z(Zt::Cone,3,4), Z(Zt::PoleVault,2,4)}, 18),
        W({Z(Zt::Football,2,4), Z(Zt::Bucket,2,3), Z(Zt::Basic,4,3)}, 15),
        W({Z(Zt::Flag,1,0), Z(Zt::Football,3,2.5f), Z(Zt::Bucket,3,2.5f), Z(Zt::PoleVault,3,2.5f)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 8: Fog - Dance Party =====
    lv = {8, "Fog 1", u8"迷霧 1 - 殭屍舞會", 50, true, false, true,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::Repeater, P::Torchwood, P::Jalapeno}, {}, 125};
    lv.waves = {
        W({Z(Zt::Basic,4,5), Z(Zt::Cone,2,5)}, 25),
        W({Z(Zt::Dancing,1,0), Z(Zt::Basic,3,5)}, 22),
        W({Z(Zt::Dancing,1,0), Z(Zt::Cone,3,4), Z(Zt::Newspaper,2,4)}, 18),
        W({Z(Zt::Dancing,2,5), Z(Zt::Football,1,0), Z(Zt::Bucket,2,3)}, 15),
        W({Z(Zt::Flag,1,0), Z(Zt::Dancing,2,3), Z(Zt::Football,2,3), Z(Zt::Bucket,3,2.5f)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 9: Fog - Gauntlet =====
    lv = {9, "Fog 2", u8"迷霧 2 - 嚴酷考驗", 50, true, true, true,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::Repeater,
         P::CherryBomb, P::Torchwood, P::Jalapeno, P::Chomper}, {}, 150};
    lv.waves = {
        W({Z(Zt::Football,1,0), Z(Zt::Dancing,1,0), Z(Zt::Cone,3,5)}, 25),
        W({Z(Zt::PoleVault,3,4), Z(Zt::Newspaper,3,4)}, 22),
        W({Z(Zt::Bucket,3,3), Z(Zt::Dancing,1,0), Z(Zt::Football,1,0)}, 18),
        W({Z(Zt::Football,2,3), Z(Zt::PoleVault,3,3), Z(Zt::Cone,4,2.5f)}, 15),
        W({Z(Zt::Flag,1,0), Z(Zt::Football,3,2), Z(Zt::Dancing,2,2.5f), Z(Zt::Bucket,4,2)}, 0, true),
    };
    levels.push_back(std::move(lv));

    // ===== Level 10: Final Stand with Boss =====
    lv = {10, "Final Stand", u8"終極決戰 - 巨人來襲", 75, true, true, true,
        {P::Peashooter, P::Sunflower, P::WallNut, P::SnowPea, P::CherryBomb,
         P::PotatoMine, P::Repeater, P::Chomper, P::Jalapeno, P::Torchwood}, {}, 300};
    lv.waves = {
        W({Z(Zt::Basic,5,4), Z(Zt::Cone,3,4)}, 25),
        W({Z(Zt::Bucket,2,3), Z(Zt::PoleVault,3,3), Z(Zt::Newspaper,3,3)}, 22),
        W({Z(Zt::Flag,1,0), Z(Zt::Football,2,3), Z(Zt::Dancing,2,3), Z(Zt::Cone,4,2.5f)}, 18),
        W({Z(Zt::Football,3,2.5f), Z(Zt::Bucket,3,2.5f), Z(Zt::PoleVault,3,2.5f)}, 15),
        W({Z(Zt::Dancing,3,3), Z(Zt::Football,2,2), Z(Zt::Newspaper,4,2)}, 12),
        // Boss wave!
        W({Z(Zt::Flag,2,0), Z(Zt::Gargantuar,1,0), Z(Zt::Football,3,2),
           Z(Zt::Dancing,2,2.5f), Z(Zt::Bucket,4,1.5f)}, 0, true),
    };
    levels.push_back(std::move(lv));

    return levels;
}

} // namespace PVZ
