#include "pvz_systems.hpp"
#include "pvz_renderer.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace PVZ {

// ============================================================================
// Entity Creation
// ============================================================================

SF3::Entity createPlant(GameState& gs, PlantType type, int row, int col) {
    auto e = gs.world.create();
    auto& data = getPlantData(type);

    gs.world.add<Plant>(e, Plant{type});
    gs.world.add<TagPlant>(e);
    gs.world.add<GridPos>(e, GridPos{row, col});
    gs.world.add<WorldPos>(e, WorldPos{{cellCenterX(col), cellCenterY(row)}});
    gs.world.add<Health>(e, Health{data.hp, data.hp});
    gs.world.add<SpriteAnim>(e, SpriteAnim{0, 0.0f, 2, 4.0f});

    if (data.attackRate > 0) {
        gs.world.add<AttackTimer>(e, AttackTimer{data.attackRate, data.attackRate * 0.5f});
    }

    switch (type) {
        case PlantType::Sunflower:
            gs.world.add<SunProducer>(e, SunProducer{5.0f, 3.0f});
            break;
        case PlantType::PotatoMine:
            gs.world.add<PotatoMineState>(e);
            break;
        case PlantType::Chomper:
            gs.world.add<ChomperState>(e);
            break;
        case PlantType::CherryBomb:
            gs.world.add<ExplosionEffect>(e, ExplosionEffect{0.0f, 1.0f, 0.0f});
            break;
        case PlantType::Jalapeno:
            gs.world.add<ExplosionEffect>(e, ExplosionEffect{0.0f, 0.8f, 0.0f});
            break;
        default:
            break;
    }

    gs.grid[row][col] = e;
    return e;
}

SF3::Entity createZombie(GameState& gs, ZombieType type, int row) {
    auto e = gs.world.create();
    auto& data = getZombieData(type);

    // Per-zombie speed variation (+-10%)
    float speedVar = 1.0f + ((std::rand() % 21) - 10) * 0.01f;

    gs.world.add<Zombie>(e, Zombie{type, false, 0.0f, data.eatRate});
    gs.world.add<TagEnemy>(e);
    gs.world.add<Health>(e, Health{data.hp, data.hp});

    float startX = GRID_OFFSET_X + GRID_COLS * CELL_W + 40.0f + (std::rand() % 60);
    gs.world.add<WorldPos>(e, WorldPos{{startX, cellCenterY(row)}});
    gs.world.add<GridPos>(e, GridPos{row, GRID_COLS});
    gs.world.add<Velocity>(e, Velocity{{-data.speed * CELL_W * speedVar, 0.0f}});
    gs.world.add<SpriteAnim>(e, SpriteAnim{0, 0.0f, 4, 3.0f});

    if (data.armorHp > 0) {
        gs.world.add<Armor>(e, Armor{data.armorHp, data.armorHp});
    }

    if (type == ZombieType::PoleVault || type == ZombieType::Newspaper || type == ZombieType::Dancing
        || type == ZombieType::Gargantuar) {
        gs.world.add<ZombieSpecial>(e);
    }

    if (type == ZombieType::Gargantuar) {
        gs.world.add<BossState>(e);
    }

    gs.world.add<Freezable>(e, Freezable{1.0f, 0.0f});
    gs.totalZombiesAlive++;
    return e;
}

SF3::Entity createProjectile(GameState& gs, ProjectileType type, float x, float y, int row, int damage) {
    auto e = gs.world.create();
    gs.world.add<Projectile>(e, Projectile{type, 300.0f, damage, row});
    gs.world.add<TagBullet>(e);
    gs.world.add<WorldPos>(e, WorldPos{{x, y}});
    gs.world.add<Velocity>(e, Velocity{{300.0f, 0.0f}});
    return e;
}

SF3::Entity createSun(GameState& gs, float x, float y, float targetY) {
    auto e = gs.world.create();
    gs.world.add<Sun>(e, Sun{targetY, true, false, {0, 0}, 10.0f});
    gs.world.add<TagSun>(e);
    gs.world.add<WorldPos>(e, WorldPos{{x, y}});
    return e;
}

SF3::Entity createLawnmower(GameState& gs, int row) {
    auto e = gs.world.create();
    float x = GRID_OFFSET_X - 30.0f;
    float y = cellCenterY(row);
    gs.world.add<Lawnmower>(e, Lawnmower{row, false, 400.0f});
    gs.world.add<TagLawnmower>(e);
    gs.world.add<WorldPos>(e, WorldPos{{x, y}});
    gs.lawnmowers[row] = e;
    return e;
}

// ============================================================================
// Plant Attack System
// ============================================================================

static bool zombieInLane(GameState& gs, int row) {
    bool found = false;
    gs.world.each<Zombie, WorldPos, GridPos>([&](SF3::Entity, Zombie&, WorldPos& wp, GridPos& gp) {
        if (gp.row == row && wp.pos.x > GRID_OFFSET_X) found = true;
    });
    return found;
}

void systemPlantAttack(GameState& gs, float dt) {
    gs.world.each<Plant, AttackTimer, GridPos, WorldPos>(
        [&](SF3::Entity e, Plant& plant, AttackTimer& at, GridPos& gp, WorldPos& wp) {
            if (!zombieInLane(gs, gp.row)) return;

            float effectiveDt = (gs.plantBoostTimer > 0) ? dt * 2.0f : dt;
            at.timer -= effectiveDt;
            if (at.timer <= 0) {
                at.timer = at.cooldown;
                auto& data = getPlantData(plant.type);

                switch (plant.type) {
                    case PlantType::Peashooter:
                        createProjectile(gs, ProjectileType::Pea,
                            wp.pos.x + 20, wp.pos.y - 5, gp.row, data.damage);
                        break;
                    case PlantType::SnowPea:
                        createProjectile(gs, ProjectileType::SnowPea,
                            wp.pos.x + 20, wp.pos.y - 5, gp.row, data.damage);
                        break;
                    case PlantType::Repeater:
                        // First pea immediately, second after short delay (via offset)
                        createProjectile(gs, ProjectileType::Pea,
                            wp.pos.x + 20, wp.pos.y - 5, gp.row, data.damage);
                        createProjectile(gs, ProjectileType::Pea,
                            wp.pos.x + 5, wp.pos.y - 5, gp.row, data.damage);
                        break;
                    default:
                        break;
                }
            }
        });
}

// ============================================================================
// Sun Production System
// ============================================================================

void systemSunProduction(GameState& gs, float dt) {
    gs.world.each<Plant, SunProducer, WorldPos>(
        [&](SF3::Entity, Plant&, SunProducer& sp, WorldPos& wp) {
            sp.timer -= dt;
            if (sp.timer <= 0) {
                sp.timer = sp.interval;
                float sunX = wp.pos.x + (std::rand() % 20 - 10);
                float sunY = wp.pos.y - 30;
                createSun(gs, sunX, sunY, wp.pos.y + 20);
            }
        });
}

// ============================================================================
// Potato Mine System
// ============================================================================

void systemPotatoMine(GameState& gs, float dt) {
    gs.world.each<Plant, PotatoMineState, GridPos, WorldPos>(
        [&](SF3::Entity e, Plant&, PotatoMineState& pm, GridPos& gp, WorldPos& wp) {
            if (!pm.armed) {
                pm.armTimer -= dt;
                if (pm.armTimer <= 0) pm.armed = true;
                return;
            }
            // Check if zombie on same cell
            bool exploded = false;
            gs.world.each<Zombie, WorldPos, GridPos, Health>(
                [&](SF3::Entity, Zombie&, WorldPos& zwp, GridPos& zgp, Health& zh) {
                    if (exploded) return;
                    if (zgp.row == gp.row) {
                        float dist = std::abs(zwp.pos.x - wp.pos.x);
                        if (dist < CELL_W * 0.5f) {
                            zh.current = 0;
                            exploded = true;
                        }
                    }
                });
            if (exploded) gs.destroyEntity(e);
        });
}

// ============================================================================
// Chomper System (shorter digest time = 20s)
// ============================================================================

void systemChomper(GameState& gs, float dt) {
    gs.world.each<Plant, ChomperState, GridPos, WorldPos>(
        [&](SF3::Entity, Plant&, ChomperState& cs, GridPos& gp, WorldPos& wp) {
            if (cs.eating) {
                cs.digestTimer -= dt;
                if (cs.digestTimer <= 0) cs.eating = false;
                return;
            }
            // Look for nearest zombie in lane within 1.5 cells
            gs.world.each<Zombie, WorldPos, GridPos, Health>(
                [&](SF3::Entity, Zombie&, WorldPos& zwp, GridPos& zgp, Health& zh) {
                    if (cs.eating) return;
                    if (zgp.row == gp.row) {
                        float dist = zwp.pos.x - wp.pos.x;
                        if (dist > 0 && dist < CELL_W * 1.5f) {
                            zh.current = 0;
                            cs.eating = true;
                            cs.digestTimer = 20.0f;  // shorter than before
                        }
                    }
                });
        });
}

// ============================================================================
// Cherry Bomb System
// ============================================================================

void systemCherryBomb(GameState& gs, float dt) {
    gs.world.each<Plant, ExplosionEffect, GridPos, WorldPos>(
        [&](SF3::Entity e, Plant& plant, ExplosionEffect& fx, GridPos& gp, WorldPos&) {
            if (plant.type != PlantType::CherryBomb) return;

            fx.timer += dt;
            fx.radius = fx.timer * 200.0f;

            if (fx.timer >= 0.3f && fx.radius > 0) {
                // Damage all zombies in 3x3 area
                gs.world.each<Zombie, WorldPos, GridPos, Health>(
                    [&](SF3::Entity, Zombie&, WorldPos&, GridPos& zgp, Health& zh) {
                        if (std::abs(zgp.row - gp.row) <= 1 && std::abs(zgp.col - gp.col) <= 1) {
                            zh.current = 0;
                        }
                    });
                gs.destroyEntity(e);
            }
        });
}

// ============================================================================
// Jalapeno System
// ============================================================================

void systemJalapeno(GameState& gs, float dt) {
    gs.world.each<Plant, ExplosionEffect, GridPos>(
        [&](SF3::Entity e, Plant& plant, ExplosionEffect& fx, GridPos& gp) {
            if (plant.type != PlantType::Jalapeno) return;

            fx.timer += dt;
            if (fx.timer >= 0.2f) {
                // Kill all zombies in lane
                gs.world.each<Zombie, GridPos, Health>(
                    [&](SF3::Entity, Zombie&, GridPos& zgp, Health& zh) {
                        if (zgp.row == gp.row) zh.current = 0;
                    });
                gs.destroyEntity(e);
            }
        });
}

// ============================================================================
// Projectile Movement
// ============================================================================

void systemProjectileMove(GameState& gs, float dt) {
    gs.world.each<Projectile, WorldPos, Velocity>(
        [&](SF3::Entity e, Projectile&, WorldPos& wp, Velocity& vel) {
            wp.pos.x += vel.vel.x * dt;
            if (wp.pos.x > SCREEN_W + 50) {
                gs.destroyEntity(e);
            }
        });
}

// ============================================================================
// Projectile vs Zombie Collision
// ============================================================================

void systemProjectileCollision(GameState& gs, float dt) {
    (void)dt;
    std::vector<SF3::Entity> hitBullets;

    gs.world.each<Projectile, WorldPos>(
        [&](SF3::Entity pe, Projectile& proj, WorldPos& pwp) {
            bool hit = false;
            gs.world.each<Zombie, WorldPos, GridPos, Health>(
                [&](SF3::Entity ze, Zombie&, WorldPos& zwp, GridPos& zgp, Health& zh) {
                    if (hit) return;
                    if (zgp.row != proj.row) return;
                    float dist = std::abs(zwp.pos.x - pwp.pos.x);
                    if (dist < 20.0f) {
                        int dmg = proj.damage;
                        if (gs.world.has<Armor>(ze)) {
                            auto& armor = gs.world.get<Armor>(ze);
                            if (armor.current > 0) {
                                int absorbed = std::min(dmg, armor.current);
                                armor.current -= absorbed;
                                dmg -= absorbed;
                                // Newspaper: armor broken = angry
                                if (armor.current <= 0 && gs.world.has<ZombieSpecial>(ze)) {
                                    auto& zombie = gs.world.get<Zombie>(ze);
                                    if (zombie.type == ZombieType::Newspaper) {
                                        gs.world.get<ZombieSpecial>(ze).isAngry = true;
                                    }
                                }
                            }
                        }
                        zh.current -= dmg;
                        zh.damageFlash = 0.15f;

                        // Snow pea freezes
                        if (proj.type == ProjectileType::SnowPea && gs.world.has<Freezable>(ze)) {
                            auto& fr = gs.world.get<Freezable>(ze);
                            fr.slowFactor = 0.5f;
                            fr.timer = 4.0f;
                        }

                        hit = true;
                    }
                });
            if (hit) hitBullets.push_back(pe);
        });

    for (auto e : hitBullets) gs.destroyEntity(e);
}

// ============================================================================
// Torchwood: upgrade peas passing through
// ============================================================================

void systemTorchwood(GameState& gs, float dt) {
    (void)dt;
    gs.world.each<Plant, GridPos, WorldPos>(
        [&](SF3::Entity, Plant& plant, GridPos& gp, WorldPos& twp) {
            if (plant.type != PlantType::Torchwood) return;

            gs.world.each<Projectile, WorldPos>(
                [&](SF3::Entity, Projectile& proj, WorldPos& pwp) {
                    if (proj.type != ProjectileType::Pea) return;
                    if (proj.row != gp.row) return;
                    float dx = pwp.pos.x - twp.pos.x;
                    if (dx > -5 && dx < 15) {
                        proj.type = ProjectileType::FirePea;
                        proj.damage *= 2;
                    }
                });
        });
}

// ============================================================================
// Zombie Movement
// ============================================================================

void systemZombieMove(GameState& gs, float dt) {
    gs.world.each<Zombie, WorldPos, Velocity, GridPos, Freezable>(
        [&](SF3::Entity, Zombie& zombie, WorldPos& wp, Velocity& vel, GridPos& gp, Freezable& fr) {
            if (zombie.eating) return;

            float speed = std::abs(vel.vel.x);
            float effective = speed * fr.slowFactor;

            wp.pos.x -= effective * dt;
            gp.col = colFromX(wp.pos.x);
        });
}

// ============================================================================
// Zombie Eat (uses per-zombie bite damage from data)
// ============================================================================

void systemZombieEat(GameState& gs, float dt) {
    gs.world.each<Zombie, WorldPos, GridPos>(
        [&](SF3::Entity, Zombie& zombie, WorldPos& wp, GridPos& gp) {
            if (gp.row < 0 || gp.row >= GRID_ROWS) return;

            int col = colFromX(wp.pos.x);
            if (col < 0 || col >= GRID_COLS) {
                zombie.eating = false;
                return;
            }

            SF3::Entity plantE = gs.grid[gp.row][col];
            if (plantE == 0 || !gs.world.alive(plantE)) {
                zombie.eating = false;
                return;
            }

            if (!gs.world.has<Health>(plantE)) {
                zombie.eating = false;
                return;
            }

            float plantX = cellCenterX(col);
            if (std::abs(wp.pos.x - plantX) > CELL_W * 0.6f) {
                zombie.eating = false;
                return;
            }

            zombie.eating = true;
            zombie.eatTimer -= dt;
            if (zombie.eatTimer <= 0) {
                zombie.eatTimer = zombie.eatRate;
                auto& zdata = getZombieData(zombie.type);
                auto& ph = gs.world.get<Health>(plantE);
                ph.current -= zdata.biteDamage;
                ph.damageFlash = 0.15f;
                if (ph.current <= 0) {
                    gs.grid[gp.row][col] = 0;
                    gs.destroyEntity(plantE);
                    zombie.eating = false;
                }
            }
        });
}

// ============================================================================
// Zombie Special Abilities
// ============================================================================

void systemZombieSpecial(GameState& gs, float dt) {
    gs.world.each<Zombie, ZombieSpecial, WorldPos, GridPos, Velocity>(
        [&](SF3::Entity, Zombie& zombie, ZombieSpecial& spec, WorldPos& wp, GridPos& gp, Velocity& vel) {
            auto& data = getZombieData(zombie.type);

            switch (zombie.type) {
                case ZombieType::PoleVault: {
                    if (spec.hasJumped) {
                        vel.vel.x = -data.speedAlt * CELL_W;
                        break;
                    }
                    int col = colFromX(wp.pos.x - CELL_W * 0.3f);
                    if (col >= 0 && col < GRID_COLS && gp.row >= 0 && gp.row < GRID_ROWS) {
                        if (gs.grid[gp.row][col] != 0) {
                            wp.pos.x -= CELL_W * 1.5f;
                            spec.hasJumped = true;
                            vel.vel.x = -data.speedAlt * CELL_W;
                        }
                    }
                    break;
                }
                case ZombieType::Newspaper: {
                    if (spec.isAngry) {
                        vel.vel.x = -data.speedAlt * CELL_W;
                    }
                    break;
                }
                case ZombieType::Dancing: {
                    if (!spec.hasSummoned) {
                        spec.summonTimer += dt;
                        if (spec.summonTimer >= 3.0f) {
                            spec.hasSummoned = true;
                            // Summon 4 backup dancers in adjacent rows
                            for (int dr = -1; dr <= 1; dr++) {
                                int r = gp.row + dr;
                                if (r >= 0 && r < GRID_ROWS) {
                                    createZombie(gs, ZombieType::Basic, r);
                                }
                            }
                        }
                    }
                    break;
                }
                case ZombieType::Gargantuar:
                    // Boss AI handles Gargantuar behavior
                    break;
                default:
                    break;
            }
        });
}

// ============================================================================
// Zombie Spawn (Wave System) - Smarter lane distribution
// ============================================================================

static int leastPopulatedLane(GameState& gs) {
    int counts[GRID_ROWS] = {};
    gs.world.each<Zombie, GridPos>([&](SF3::Entity, Zombie&, GridPos& gp) {
        if (gp.row >= 0 && gp.row < GRID_ROWS) counts[gp.row]++;
    });
    int best = 0;
    for (int i = 1; i < GRID_ROWS; i++) {
        if (counts[i] < counts[best]) best = i;
    }
    // Add some randomness: 60% pick least populated, 40% random
    if (std::rand() % 100 < 40) return std::rand() % GRID_ROWS;
    return best;
}

void systemZombieSpawn(GameState& gs, float dt) {
    if (!gs.levelDef) return;
    if (gs.levelComplete || gs.levelFailed) return;
    if (gs.currentWave >= static_cast<int>(gs.levelDef->waves.size())) return;

    auto& wave = gs.levelDef->waves[gs.currentWave];
    if (gs.waveEntryIndex >= static_cast<int>(wave.entries.size())) {
        gs.waveTimer += dt;
        if (gs.waveTimer >= wave.delayAfter || (wave.delayAfter > 0 && gs.totalZombiesAlive <= 0)) {
            gs.currentWave++;
            gs.waveTimer = 0;
            gs.waveEntryIndex = 0;
            gs.zombiesSpawned = 0;
            // Signal new wave for announcements
            gs.newWaveStarted = true;
        }
        return;
    }

    auto& entry = wave.entries[gs.waveEntryIndex];
    gs.spawnTimer -= dt;
    if (gs.spawnTimer <= 0) {
        int row = leastPopulatedLane(gs);
        createZombie(gs, entry.type, row);
        gs.zombiesSpawned++;
        gs.spawnTimer = entry.delayBetween;

        if (gs.zombiesSpawned >= entry.count) {
            gs.waveEntryIndex++;
            gs.zombiesSpawned = 0;
        }
    }
}

// ============================================================================
// Sun Systems
// ============================================================================

void systemSunFall(GameState& gs, float dt) {
    gs.world.each<Sun, WorldPos>(
        [&](SF3::Entity e, Sun& sun, WorldPos& wp) {
            if (sun.collecting) {
                SF3::Vec2 dir = {sun.collectTarget.x - wp.pos.x, sun.collectTarget.y - wp.pos.y};
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len < 10.0f) {
                    gs.sunCount += 25;
                    gs.destroyEntity(e);
                    return;
                }
                float speed = 600.0f;
                wp.pos.x += (dir.x / len) * speed * dt;
                wp.pos.y += (dir.y / len) * speed * dt;
                return;
            }

            if (sun.falling) {
                wp.pos.y += 50.0f * dt;
                if (wp.pos.y >= sun.targetY) {
                    wp.pos.y = sun.targetY;
                    sun.falling = false;
                }
            }

            sun.lifetime -= dt;
            if (sun.lifetime <= 0) {
                gs.destroyEntity(e);
            }
        });
}

void systemSunCollect(GameState& gs, float dt) {
    (void)dt;
}

void systemSkySunDrop(GameState& gs, float dt) {
    if (!gs.levelDef || !gs.levelDef->hasSkyDrop) return;
    gs.skyDropTimer -= dt;
    if (gs.skyDropTimer <= 0) {
        gs.skyDropTimer = 5.0f + (std::rand() % 40) * 0.1f;
        float x = GRID_OFFSET_X + (std::rand() % static_cast<int>(GRID_COLS * CELL_W));
        float targetY = GRID_OFFSET_Y + (std::rand() % static_cast<int>(GRID_ROWS * CELL_H));
        createSun(gs, x, -20.0f, targetY);
    }
}

// ============================================================================
// Lawnmower
// ============================================================================

void systemLawnmower(GameState& gs, float dt) {
    gs.world.each<Lawnmower, WorldPos>(
        [&](SF3::Entity e, Lawnmower& lm, WorldPos& wp) {
            if (lm.triggered) {
                wp.pos.x += lm.speed * dt;
                gs.world.each<Zombie, WorldPos, GridPos, Health>(
                    [&](SF3::Entity, Zombie&, WorldPos& zwp, GridPos& zgp, Health& zh) {
                        if (zgp.row == lm.row && std::abs(zwp.pos.x - wp.pos.x) < 30.0f) {
                            zh.current = 0;
                        }
                    });
                if (wp.pos.x > SCREEN_W + 50) gs.destroyEntity(e);
                return;
            }
            gs.world.each<Zombie, WorldPos, GridPos>(
                [&](SF3::Entity, Zombie&, WorldPos& zwp, GridPos& zgp) {
                    if (zgp.row == lm.row && zwp.pos.x <= wp.pos.x + 10) {
                        lm.triggered = true;
                    }
                });
        });
}

// ============================================================================
// Animation
// ============================================================================

void systemAnimation(GameState& gs, float dt) {
    gs.world.each<SpriteAnim>(
        [&](SF3::Entity, SpriteAnim& anim) {
            anim.timer += dt;
            if (anim.timer >= 1.0f / anim.fps) {
                anim.timer = 0;
                anim.frameIndex = (anim.frameIndex + 1) % anim.frameCount;
            }
        });
    // Decay damage flash
    gs.world.each<Health>(
        [&](SF3::Entity, Health& h) {
            if (h.damageFlash > 0) h.damageFlash -= dt;
        });

    // Combo timer decay
    gs.comboTimer += dt;
    if (gs.comboTimer > 2.0f) {
        gs.comboCount = 0;  // combo broken
    }
    if (gs.comboDisplayTimer > 0) {
        gs.comboDisplayTimer -= dt;
    }
}

// ============================================================================
// Freeze Decay
// ============================================================================

void systemFreeze(GameState& gs, float dt) {
    gs.world.each<Freezable>(
        [&](SF3::Entity, Freezable& fr) {
            if (fr.timer > 0) {
                fr.timer -= dt;
                if (fr.timer <= 0) {
                    fr.slowFactor = 1.0f;
                    fr.timer = 0;
                }
            }
        });
}

// ============================================================================
// Explosions visual
// ============================================================================

void systemExplosions(GameState& gs, float dt) {
    gs.world.each<ExplosionEffect>(
        [&](SF3::Entity e, ExplosionEffect& fx) {
            fx.timer += dt;
            if (fx.timer >= fx.duration) {
                gs.destroyEntity(e);
            }
        });
}

// ============================================================================
// Win/Lose Check + coin drops
// ============================================================================

void systemWinLose(GameState& gs, float dt) {
    (void)dt;
    if (gs.levelComplete || gs.levelFailed) return;

    // Check lose
    gs.world.each<Zombie, WorldPos>(
        [&](SF3::Entity, Zombie&, WorldPos& wp) {
            if (wp.pos.x < GRID_OFFSET_X - 60) {
                gs.levelFailed = true;
            }
        });

    // Check win
    if (gs.levelDef && gs.currentWave >= static_cast<int>(gs.levelDef->waves.size())) {
        int alive = 0;
        gs.world.each<Zombie>([&](SF3::Entity, Zombie&) { alive++; });
        if (alive == 0) {
            gs.levelComplete = true;
            gs.coinsEarned = gs.levelDef->coinReward;
        }
    }

    // Remove dead zombies + coin drops
    std::vector<SF3::Entity> dead;
    gs.world.each<Zombie, Health>(
        [&](SF3::Entity e, Zombie& z, Health& h) {
            if (h.current <= 0) dead.push_back(e);
        });
    for (auto e : dead) {
        gs.totalZombiesAlive--;
        gs.totalZombiesKilled++;

        // Combo system
        gs.comboCount++;
        gs.comboTimer = 0.0f;
        gs.maxCombo = std::max(gs.maxCombo, gs.comboCount);
        gs.comboDisplayTimer = 2.0f;

        // Coin value from zombie data + drop spawning
        if (gs.world.has<Zombie>(e)) {
            auto& z = gs.world.get<Zombie>(e);
            gs.coinsEarned += getZombieData(z.type).coinValue;

            // Drop spawning chance
            if (gs.world.has<WorldPos>(e)) {
                auto& zwp = gs.world.get<WorldPos>(e);
                int roll = std::rand() % 100;
                if (roll < 3) {
                    // 3% chance: random power-up (PlantBoost, SunBurst, or FreezeAll)
                    int puRoll = std::rand() % 3;
                    DropType puType;
                    if (puRoll == 0) puType = DropType::PlantBoost;
                    else if (puRoll == 1) puType = DropType::SunBurst;
                    else puType = DropType::FreezeAll;
                    createDrop(gs, puType, zwp.pos.x, zwp.pos.y);
                } else if (roll < 8) {
                    // 5% chance: Diamond (rolls 3-7)
                    createDrop(gs, DropType::Diamond, zwp.pos.x, zwp.pos.y);
                } else if (roll < 38) {
                    // 30% chance: Coin (rolls 8-37)
                    createDrop(gs, DropType::Coin, zwp.pos.x, zwp.pos.y);
                }
            }
        }
        gs.destroyEntity(e);
    }

    // Remove dead plants
    std::vector<SF3::Entity> deadPlants;
    gs.world.each<Plant, Health>(
        [&](SF3::Entity e, Plant&, Health& h) {
            if (h.current <= 0) deadPlants.push_back(e);
        });
    for (auto e : deadPlants) {
        gs.destroyEntity(e);
    }
}

// ============================================================================
// Drop Creation
// ============================================================================

SF3::Entity createDrop(GameState& gs, DropType type, float x, float y) {
    auto e = gs.world.create();
    gs.world.add<Drop>(e, Drop{type, y + 30.0f, true, false, {0, 0}, 12.0f});
    gs.world.add<WorldPos>(e, WorldPos{{x, y - 20.0f}});
    gs.world.add<TagDrop>(e);
    return e;
}

// ============================================================================
// Boss AI System (Gargantuar)
// ============================================================================

void systemBossAI(GameState& gs, float dt) {
    gs.world.each<Zombie, BossState, WorldPos, GridPos, Health, Velocity>(
        [&](SF3::Entity e, Zombie& zombie, BossState& boss, WorldPos& wp, GridPos& gp, Health& zh, Velocity& vel) {
            if (zombie.type != ZombieType::Gargantuar) return;

            // Phase transition: below 50% HP
            if (boss.phase == 1 && zh.current <= zh.max / 2) {
                boss.phase = 2;
                // Slightly faster in phase 2
                vel.vel.x *= 1.3f;
            }

            // Throw Imp at 50% HP (once only)
            if (!boss.hasThrown && zh.current <= zh.max / 2) {
                boss.hasThrown = true;
                // Throw imp 3 cells ahead (to the left)
                int targetCol = gp.col - 3;
                if (targetCol < 0) targetCol = 0;
                float impX = cellCenterX(targetCol);
                auto imp = createZombie(gs, ZombieType::Imp, gp.row);
                if (gs.world.has<WorldPos>(imp)) {
                    gs.world.get<WorldPos>(imp).pos.x = impX;
                }
                if (gs.world.has<GridPos>(imp)) {
                    gs.world.get<GridPos>(imp).col = targetCol;
                }
            }

            // Smash attack: every 4 seconds
            boss.smashTimer += dt;
            if (boss.smashTimer >= boss.smashCooldown) {
                // Check for plant in same cell or adjacent cell in front (one col to the left)
                bool smashed = false;
                gs.world.each<Plant, GridPos, Health>(
                    [&](SF3::Entity pe, Plant&, GridPos& pgp, Health& ph) {
                        if (smashed) return;
                        if (pgp.row != gp.row) return;
                        // Same cell or adjacent cell in front (col or col-1)
                        if (pgp.col == gp.col || pgp.col == gp.col - 1) {
                            ph.current = 0;  // Instantly destroy
                            smashed = true;
                        }
                    });
                if (smashed) {
                    boss.smashTimer = 0.0f;
                }
            }
        });
}

// ============================================================================
// Drop System
// ============================================================================

void systemDrops(GameState& gs, float dt) {
    gs.world.each<Drop, WorldPos>(
        [&](SF3::Entity e, Drop& drop, WorldPos& wp) {
            // Collecting: move toward collection point
            if (drop.collecting) {
                SF3::Vec2 dir = {drop.collectTarget.x - wp.pos.x, drop.collectTarget.y - wp.pos.y};
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len < 10.0f) {
                    // Collected - apply effect based on type
                    switch (drop.type) {
                        case DropType::Coin:
                            gs.coinsEarned += 10;
                            break;
                        case DropType::Diamond:
                            gs.coinsEarned += 50;
                            break;
                        case DropType::PlantBoost:
                            gs.plantBoostTimer = 10.0f;
                            break;
                        case DropType::SunBurst:
                            gs.sunCount += 100;
                            break;
                        case DropType::FreezeAll:
                            gs.freezeAllTimer = 5.0f;
                            break;
                    }
                    gs.destroyEntity(e);
                    return;
                }
                float speed = 500.0f;
                wp.pos.x += (dir.x / len) * speed * dt;
                wp.pos.y += (dir.y / len) * speed * dt;
                return;
            }

            // Falling
            if (drop.falling) {
                wp.pos.y += 50.0f * dt;
                if (wp.pos.y >= drop.targetY) {
                    wp.pos.y = drop.targetY;
                    drop.falling = false;
                }
            }

            // Lifetime decay
            drop.lifetime -= dt;
            if (drop.lifetime <= 0) {
                gs.destroyEntity(e);
            }
        });
}

// ============================================================================
// Power-Up System
// ============================================================================

void systemPowerUps(GameState& gs, float dt) {
    // Decay power-up timers
    if (gs.plantBoostTimer > 0) {
        gs.plantBoostTimer -= dt;
        if (gs.plantBoostTimer < 0) gs.plantBoostTimer = 0;
    }

    if (gs.freezeAllTimer > 0) {
        gs.freezeAllTimer -= dt;
        if (gs.freezeAllTimer < 0) gs.freezeAllTimer = 0;

        // Freeze all zombies
        gs.world.each<Freezable>(
            [&](SF3::Entity, Freezable& fr) {
                fr.slowFactor = 0.3f;
                fr.timer = 1.0f;
            });
    }
}

} // namespace PVZ
