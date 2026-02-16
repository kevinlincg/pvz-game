// ============================================
// Plant Legends - Game Implementation
// ============================================

#include "game/game.hpp"
#include "game/projectile.hpp"
#include "lua/lua_manager.hpp"
#include <iostream>
#include <algorithm>
#include <random>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace PL {

// 全局遊戲實例
static Game* s_game = nullptr;

Game& getGame() {
    if (!s_game) {
        s_game = new Game();
    }
    return *s_game;
}

Game::Game() {
    s_game = this;
}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    std::cout << "[Game] Initializing..." << std::endl;
    
    // 載入配置
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "config");
    if (lua_istable(L, -1)) {
        // 讀取網格配置
        lua_getfield(L, -1, "global");
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "grid_cols");
            if (lua_isnumber(L, -1)) {
                gridConfig.cols = (i32)lua_tointeger(L, -1);
            }
            lua_pop(L, 1);
            
            lua_getfield(L, -1, "grid_rows");
            if (lua_isnumber(L, -1)) {
                gridConfig.rows = (i32)lua_tointeger(L, -1);
            }
            lua_pop(L, 1);
            
            lua_getfield(L, -1, "cell_width");
            if (lua_isnumber(L, -1)) {
                gridConfig.cellWidth = (f32)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
            
            lua_getfield(L, -1, "cell_height");
            if (lua_isnumber(L, -1)) {
                gridConfig.cellHeight = (f32)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
            
            // 初始陽光
            lua_getfield(L, -1, "starting_sun");
            if (lua_isnumber(L, -1)) {
                sun = (i32)lua_tointeger(L, -1);
            }
            lua_pop(L, 1);
            
            // 陽光生成間隔
            lua_getfield(L, -1, "sun_generation_interval");
            if (lua_isnumber(L, -1)) {
                sunInterval = (f32)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);  // pop global
    }
    lua_pop(L, 1);  // pop config
    
    std::cout << "[Game] Grid: " << gridConfig.cols << "x" << gridConfig.rows << std::endl;
    std::cout << "[Game] Cell size: " << gridConfig.cellWidth << "x" << gridConfig.cellHeight << std::endl;
    std::cout << "[Game] Initial sun: " << sun << std::endl;
    
    state = GameState::Menu;
    return true;
}

void Game::shutdown() {
    std::cout << "[Game] Shutting down..." << std::endl;
    
    plants.clear();
    enemies.clear();
    projectiles.clear();
    grid.clear();
    waves.clear();
    
    if (s_game == this) {
        s_game = nullptr;
    }
}

void Game::update(f32 dt) {
    if (state != GameState::Playing) return;
    
    levelTimer += dt;
    
    updateSun(dt);
    updateWaves(dt);
    updatePlants(dt);
    updateEnemies(dt);
    updateProjectiles(dt);
    updateCombat(dt);
    
    cleanupDeadEntities();
    
    // 檢查勝利/失敗條件
    // TODO: 實現勝利/失敗檢測
}

void Game::render() {
    // 渲染將由渲染系統處理
}

Vec2 Game::gridToWorld(const GridCoord& coord) const {
    return Vec2(
        gridConfig.offsetX + coord.col * gridConfig.cellWidth + gridConfig.cellWidth / 2,
        gridConfig.offsetY + coord.row * gridConfig.cellHeight + gridConfig.cellHeight / 2
    );
}

GridCoord Game::worldToGrid(const Vec2& pos) const {
    return GridCoord(
        (i32)((pos.x - gridConfig.offsetX) / gridConfig.cellWidth),
        (i32)((pos.y - gridConfig.offsetY) / gridConfig.cellHeight)
    );
}

bool Game::isValidGridPosition(const GridCoord& coord) const {
    return coord.col >= 0 && coord.col < gridConfig.cols &&
           coord.row >= 0 && coord.row < gridConfig.rows;
}

bool Game::isGridOccupied(const GridCoord& coord) const {
    return grid.find(gridToKey(coord)) != grid.end();
}

bool Game::placePlant(const std::string& plantId, const GridCoord& coord) {
    if (!isValidGridPosition(coord) || isGridOccupied(coord)) {
        return false;
    }
    
    // 創建植物
    auto plant = std::make_shared<Plant>(plantId);
    
    // 檢查陽光是否足夠
    if (sun < plant->getCost()) {
        std::cout << "[Game] Not enough sun! Need " << plant->getCost() << ", have " << sun << std::endl;
        return false;
    }
    
    // 扣除陽光
    sun -= plant->getCost();
    
    // 設置位置
    plant->setGridPosition(coord);
    plant->setPosition(gridToWorld(coord));
    
    // 添加到容器
    plants.push_back(plant);
    grid[gridToKey(coord)] = plant;
    
    std::cout << "[Game] Placed " << plantId << " at (" << coord.col << ", " << coord.row << ")" << std::endl;
    return true;
}

void Game::removePlant(const GridCoord& coord) {
    auto it = grid.find(gridToKey(coord));
    if (it != grid.end()) {
        it->second->destroy();
        grid.erase(it);
    }
}

PlantPtr Game::getPlantAt(const GridCoord& coord) const {
    auto it = grid.find(gridToKey(coord));
    return it != grid.end() ? it->second : nullptr;
}

void Game::spawnEnemy(const std::string& enemyId, i32 row) {
    auto enemy = std::make_shared<Enemy>(enemyId);
    
    // 設置位置（從右邊開始）
    Vec2 pos(1200.0f, gridConfig.offsetY + row * gridConfig.cellHeight + gridConfig.cellHeight / 2);
    enemy->setPosition(pos);
    enemy->setRow(row);
    
    enemies.push_back(enemy);
    
    std::cout << "[Game] Spawned " << enemyId << " at row " << row << std::endl;
}

bool Game::spendSun(i32 amount) {
    if (sun >= amount) {
        sun -= amount;
        return true;
    }
    return false;
}

bool Game::loadLevel(const std::string& levelId) {
    LuaManager& lua = LuaManager::instance();
    lua_State* L = lua.getState();
    
    lua_getglobal(L, "levels");
    if (!lua_istable(L, -1)) {
        std::cerr << "[Game] levels table not found" << std::endl;
        lua_pop(L, 1);
        return false;
    }
    
    lua_getfield(L, -1, levelId.c_str());
    if (!lua_istable(L, -1)) {
        std::cerr << "[Game] Level " << levelId << " not found" << std::endl;
        lua_pop(L, 2);
        return false;
    }
    
    currentLevelId = levelId;
    waves.clear();
    
    // 讀取初始陽光
    lua_getfield(L, -1, "initial");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "sun");
        if (lua_isnumber(L, -1)) {
            sun = (i32)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    // 讀取波次
    lua_getfield(L, -1, "waves");
    if (lua_istable(L, -1)) {
        i32 waveIndex = 1;
        while (true) {
            lua_rawgeti(L, -1, waveIndex);
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                break;
            }
            
            WaveConfig wave;
            
            lua_getfield(L, -1, "time");
            if (lua_isnumber(L, -1)) {
                wave.time = (f32)lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
            
            lua_getfield(L, -1, "enemies");
            if (lua_istable(L, -1)) {
                i32 enemyIndex = 1;
                while (true) {
                    lua_rawgeti(L, -1, enemyIndex);
                    if (!lua_istable(L, -1)) {
                        lua_pop(L, 1);
                        break;
                    }
                    
                    lua_getfield(L, -1, "type");
                    std::string enemyType = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
                    lua_pop(L, 1);
                    
                    lua_getfield(L, -1, "count");
                    i32 count = lua_isnumber(L, -1) ? (i32)lua_tointeger(L, -1) : 1;
                    lua_pop(L, 1);
                    
                    if (!enemyType.empty()) {
                        wave.enemies.push_back({enemyType, count});
                    }
                    
                    lua_pop(L, 1);
                    enemyIndex++;
                }
            }
            lua_pop(L, 1);  // pop enemies
            
            waves.push_back(wave);
            
            lua_pop(L, 1);  // pop wave
            waveIndex++;
        }
    }
    lua_pop(L, 1);  // pop waves
    
    lua_pop(L, 2);  // pop level and levels
    
    std::cout << "[Game] Loaded level " << levelId << " with " << waves.size() << " waves" << std::endl;
    return true;
}

void Game::startLevel() {
    state = GameState::Playing;
    levelTimer = 0.0f;
    currentWave = 0;
    levelStarted = true;
    
    std::cout << "[Game] Level started!" << std::endl;
}

void Game::updateSun(f32 dt) {
    sunTimer += dt;
    
    if (sunTimer >= sunInterval) {
        sun += 25;
        sunTimer = 0.0f;
        std::cout << "[Game] Generated 25 sun. Total: " << sun << std::endl;
    }
}

void Game::updateWaves(f32 dt) {
    if (currentWave >= waves.size()) return;
    
    auto& wave = waves[currentWave];
    
    if (levelTimer >= wave.time) {
        std::cout << "[Game] Spawning wave " << (currentWave + 1) << std::endl;
        
        // 生成敵人
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<i32> rowDist(0, gridConfig.rows - 1);
        
        for (auto& [enemyId, count] : wave.enemies) {
            for (i32 i = 0; i < count; i++) {
                i32 row = rowDist(gen);
                spawnEnemy(enemyId, row);
            }
        }
        
        currentWave++;
    }
}

void Game::updatePlants(f32 dt) {
    findPlantTargets();
    
    for (auto& plant : plants) {
        if (plant->isAlive()) {
            plant->update(dt);
        }
    }
}

void Game::updateEnemies(f32 dt) {
    findEnemyTargets();
    
    for (auto& enemy : enemies) {
        if (enemy->isAlive()) {
            enemy->update(dt);
            
            // 檢查是否到達終點
            if (enemy->getPosition().x < 50.0f) {
                std::cout << "[Game] Enemy reached the end! Game Over!" << std::endl;
                state = GameState::GameOver;
            }
        }
    }
}

void Game::updateProjectiles(f32 dt) {
    for (auto& proj : projectiles) {
        if (proj->isAlive()) {
            proj->update(dt);
        }
    }
}

void Game::updateCombat(f32 dt) {
    // 植物攻擊
    for (auto& plant : plants) {
        if (!plant->isAlive() || !plant->canAttack()) continue;
        
        auto target = plant->getTarget();
        if (target && target->isAlive()) {
            // 生成投射物
            spawnProjectile(plant, target, plant->getStats().damage);
            plant->attack();
        }
    }
    
    // 敵人攻擊
    for (auto& enemy : enemies) {
        if (!enemy->isAlive() || !enemy->canAttack()) continue;
        
        enemy->attack();
    }
}

void Game::spawnProjectile(PlantPtr source, EnemyPtr target, f32 damage) {
    auto proj = std::make_shared<Projectile>(source->getPosition(), target, damage);
    projectiles.push_back(proj);
}

void Game::findPlantTargets() {
    for (auto& plant : plants) {
        if (!plant->isAlive()) continue;
        
        EnemyPtr closest = nullptr;
        f32 closestDist = plant->getStats().range;
        
        for (auto& enemy : enemies) {
            if (!enemy->isAlive()) continue;
            
            f32 dist = plant->getPosition().distanceSq(enemy->getPosition());
            if (dist < closestDist * closestDist) {
                closestDist = std::sqrt(dist);
                closest = enemy;
            }
        }
        
        plant->setTarget(closest);
    }
}

void Game::findEnemyTargets() {
    for (auto& enemy : enemies) {
        if (!enemy->isAlive()) continue;
        
        // 找同一行的植物
        PlantPtr target = nullptr;
        f32 minDist = 100.0f;  // 攻擊範圍
        
        for (auto& plant : plants) {
            if (!plant->isAlive()) continue;
            
            // 檢查是否同一行
            if (plant->getGridPosition().row == enemy->getRow()) {
                f32 dist = enemy->getPosition().distance(plant->getPosition());
                if (dist < minDist) {
                    minDist = dist;
                    target = plant;
                }
            }
        }
        
        enemy->setTargetPlant(target);
    }
}

void Game::cleanupDeadEntities() {
    // 清理死亡的植物
    plants.erase(
        std::remove_if(plants.begin(), plants.end(),
            [](const PlantPtr& p) { return !p->isAlive(); }),
        plants.end()
    );
    
    // 清理死亡的敵人
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const EnemyPtr& e) { return !e->isAlive(); }),
        enemies.end()
    );
    
    // 清理完成的投射物
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const ProjectilePtr& p) { return !p->isAlive(); }),
        projectiles.end()
    );
    
    // 清理網格中的死亡植物
    for (auto it = grid.begin(); it != grid.end(); ) {
        if (!it->second->isAlive()) {
            it = grid.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace PL
