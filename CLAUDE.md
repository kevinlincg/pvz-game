# CLAUDE.md

## Project Overview

Plants vs Zombies clone built with SF3 (SoulFire 3.0) engine. ECS-based architecture with procedural pixel art rendering (no external sprite assets). All visuals are drawn at runtime using `Graphics::drawRect`, `drawCircleFilled`, `drawLine`, etc.

## Build Commands

```bash
# Desktop (macOS Apple Silicon)
cmake -B build -G Ninja -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
./build/pvz

# Web (Emscripten) - requires sf3-engine build-web first
source ~/emsdk/emsdk_env.sh
emcmake cmake -B build-web -G Ninja \
    -DSF3_BUILD_DIR=../sf3-engine/build-web \
    -DSF3_LIBRARY=../sf3-engine/build-web/libsf3.a
cmake --build build-web

# Source mode (includes full sf3 source for debugging)
cmake -B build -G Ninja -DPVZ_SF3_SOURCE=ON -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

## Architecture

### ECS Pattern
All game entities use SF3's `World`/`Entity`/component system. Components are plain structs in `pvz_components.hpp`. Systems in `pvz_systems.cpp` iterate entities with `world.each<Component1, Component2>(...)`.

### File Responsibilities

| File | Purpose |
|------|---------|
| `pvz_data.hpp` | Static data tables: plant stats, zombie stats, level definitions, grid constants |
| `pvz_components.hpp` | ECS component structs: `Plant`, `Zombie`, `Projectile`, `Health`, `WorldPos`, etc. |
| `pvz_systems.hpp/cpp` | All ECS systems: attack, movement, collision, spawning, boss AI, combos, drops, power-ups |
| `pvz_renderer.hpp/cpp` | Procedural pixel art: `drawPlant()`, `drawZombie()`, `drawDrop()`, grass cells, dirt path |
| `pvz_scenes.hpp/cpp` | 5 scenes: `MenuScene`, `LevelSelectScene`, `ShopScene`, `GameScene`, `GameOverScene` |
| `pvz_hud.hpp/cpp` | In-game HUD: seed packet bar, sun counter, wave progress, shovel tool |
| `pvz_font.hpp/cpp` | Font manager: cross-platform font loading, text draw helpers, subset font for web |
| `main.cpp` | Entry point, scene registration, Emscripten main loop adapter |

### Game Flow
```
MenuScene -> LevelSelectScene -> GameScene -> GameOverScene
                |                                  |
                v                                  v
            ShopScene                    (back to LevelSelect)
```

### Key Data Structures

- `GameState` (in `pvz_scenes.hpp`): Central game state - ECS world, sun count, wave info, grid occupancy, combo tracking
- `PlantData` / `ZombieData` (in `pvz_data.hpp`): Static stat tables indexed by enum
- `LevelDef` (in `pvz_data.hpp`): Level config with waves, available plants, theme flags
- `Renderer` (in `pvz_renderer.hpp`): Static methods for drawing all entity types

### SF3 Engine Integration

SF3 engine is expected at `../sf3-engine/` (sibling directory). Two build modes:
- **Prebuilt** (default): Links `libsf3.a` + headers. Fast compile.
- **Source** (`-DPVZ_SF3_SOURCE=ON`): `add_subdirectory` for full debugging.

Key SF3 APIs used:
- `SF3::Graphics::drawRect/drawCircleFilled/drawLine` - All rendering
- `SF3::Font::load/draw` - Text rendering
- `SF3::SceneManager` - Scene flow with fade transitions
- `SF3::Input::mousePressed/mousePos/keyPressed` - Input
- `SF3::Scheduler::instance().after/every` - Timers
- `SF3::TweenManager` - Animations
- `SF3::Camera2D::shake` - Screen shake
- `SF3::ParticleSystem` - Particle effects

### Emscripten Notes

- Main loop uses `emscripten_set_main_loop()` with extracted `mainLoop()` function
- Canvas requires CSS `width`/`height` for SDL3 to detect external sizing (SDL3 resets canvas to 1x1 during init)
- Font subset (`pvz_font_subset.ttf`, 214KB) embedded via `--preload-file`
- Disabled modules for web: NET, PACKAGE, GAMEPAD, SPATIAL_AUDIO
- Uses `-sUSE_SDL=0` (SF3 provides its own SDL3 via FetchContent)
- Deployed to Cloudflare Pages: https://pvz-game.pages.dev/

## Conventions

- All game-specific code is in `namespace PVZ`
- Traditional Chinese (zh-TW) for all user-facing text
- Plant/Zombie data is defined as `constexpr`/`inline` tables in `pvz_data.hpp`
- Renderer methods are static, no state - they take position + frame + HP and draw
- Systems follow naming pattern: `system<Name>()` (e.g., `systemPlantAttack()`, `systemBossAI()`)
