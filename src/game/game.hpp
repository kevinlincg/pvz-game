// ============================================
// Plant Legends - Game 主類
// ============================================

#pragma once

#include "core/types.hpp"
#include "game/plant.hpp"
#include "game/enemy.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace PL {

// 網格配置
struct GridConfig {
    i32 cols = 9;
    i32 rows = 5;
    f32 cellWidth = 80.0f;
    f32 cellHeight = 100.0f;
    f32 offsetX = 100.0f;
    f32 offsetY = 100.0f;
};

// 遊戲狀態
enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver,
    Victory
};

// 波次配置
struct WaveConfig {
    f32 time = 0.0f;
    std::vector<std::pair<std::string, i32>> enemies;  // enemyId, count
};

class Game {
public:
    Game();
    ~Game();
    
    // 初始化
    bool initialize();
    void shutdown();
    
    // 遊戲循環
    void update(f32 dt);
    void render();
    
    // 狀態
    GameState getState() const { return state; }
    void setState(GameState s) { state = s; }
    
    // 網格
    Vec2 gridToWorld(const GridCoord& coord) const;
    GridCoord worldToGrid(const Vec2& pos) const;
    bool isValidGridPosition(const GridCoord& coord) const;
    bool isGridOccupied(const GridCoord& coord) const;
    
    // 植物
    bool placePlant(const std::string& plantId, const GridCoord& coord);
    void removePlant(const GridCoord& coord);
    PlantPtr getPlantAt(const GridCoord& coord) const;
    const std::vector<PlantPtr>& getPlants() const { return plants; }
    
    // 敵人
    void spawnEnemy(const std::string& enemyId, i32 row);
    const std::vector<EnemyPtr>& getEnemies() const { return enemies; }
    
    // 投射物
    const std::vector<ProjectilePtr>& getProjectiles() const { return projectiles; }
    
    // 資源
    i32 getSun() const { return sun; }
    void addSun(i32 amount) { sun += amount; }
    bool spendSun(i32 amount);
    
    // 關卡
    bool loadLevel(const std::string& levelId);
    void startLevel();
    
    // 戰鬥
    void updateCombat(f32 dt);
    
    // 投射物
    void spawnProjectile(PlantPtr source, EnemyPtr target, f32 damage);
    
private:
    GameState state = GameState::Menu;
    
    // 網格
    GridConfig gridConfig;
    std::unordered_map<i32, PlantPtr> grid;  // key = row * cols + col
    
    // 實體
    std::vector<PlantPtr> plants;
    std::vector<EnemyPtr> enemies;
    std::vector<ProjectilePtr> projectiles;
    
    // 資源
    i32 sun = 50;
    
    // 關卡
    std::string currentLevelId;
    std::vector<WaveConfig> waves;
    i32 currentWave = 0;
    f32 levelTimer = 0.0f;
    bool levelStarted = false;
    
    // 陽光生成
    f32 sunTimer = 0.0f;
    f32 sunInterval = 5.0f;
    
    // 輔助函數
    void updatePlants(f32 dt);
    void updateEnemies(f32 dt);
    void updateProjectiles(f32 dt);
    void updateSun(f32 dt);
    void updateWaves(f32 dt);
    
    void findPlantTargets();
    void findEnemyTargets();
    
    void cleanupDeadEntities();
    
    i32 gridToKey(const GridCoord& coord) const {
        return coord.row * gridConfig.cols + coord.col;
    }
};

// 全局遊戲實例
Game& getGame();

} // namespace PL
