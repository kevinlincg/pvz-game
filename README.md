# Plants vs Zombies - SF3 Engine

A full-featured Plants vs Zombies clone built with the [SF3 (SoulFire 3.0)](https://github.com/kevinlincg/sf3-engine) 2D game engine. Features ECS architecture, procedurally-generated pixel art (zero external sprite assets), 10 plants, 10 zombies (including a boss), 10 levels with day/night/fog themes, a shop system, and save/load progression.

**Play now:** https://pvz-game.pages.dev/

## Screenshots

The game uses entirely procedural pixel art - every plant, zombie, projectile, and UI element is drawn at runtime using primitive shapes (rectangles, circles, lines).

## Features

### Plants (10 types)

| Plant | Cost | Mechanic |
|-------|------|----------|
| Sunflower | 50 | Produces 25 sun every 5s |
| Peashooter | 100 | Shoots pea every 1.4s |
| Wall-nut | 50 | High HP (800) wall |
| Snow Pea | 175 | Freezing pea (50% slow) |
| Cherry Bomb | 150 | Explodes 3x3 area |
| Potato Mine | 25 | Arms after 15s, one-shot kill |
| Repeater | 200 | Shoots 2 peas per attack |
| Chomper | 150 | Eats zombie, 30s digest |
| Jalapeno | 125 | Burns entire lane |
| Torchwood | 175 | Upgrades passing peas to fire peas (2x damage) |

### Zombies (10 types)

| Zombie | HP | Special |
|--------|----|---------|
| Basic | 100 | None |
| Conehead | 100+200 | Cone armor |
| Buckethead | 100+500 | Bucket armor |
| Flag | 100 | Appears at wave start |
| Newspaper | 100+100 | Speeds up when paper destroyed |
| Pole Vaulter | 175 | Jumps over first plant |
| Football | 300+100 | Fast + armored |
| Dancing | 200 | Summons 4 backup dancers |
| Imp | 80 | Small, fast (thrown by Gargantuar) |
| Gargantuar | 1500+300 | Boss: smash attack, throws Imp at 50% HP |

### Levels (10)

- **Levels 1-5:** Day theme - tutorial through advanced mechanics
- **Levels 6-7:** Night theme - dark overlay with limited visibility
- **Levels 8-9:** Fog theme - right-side fog obscuring zombie approach
- **Level 10:** Final Stand - night + fog, Gargantuar boss wave

### Game Systems

- **ECS Architecture** - All game entities managed through SF3's World/Entity/Component system
- **Wave-based Spawning** - Each level defines multiple waves with escalating difficulty
- **Combo System** - Chain kills within 2s for drop bonuses (coins, diamonds, power-ups)
- **5 Drop Types** - Coin, Diamond, Plant Boost, Sun Burst, Freeze All
- **Lawnmowers** - One per lane, auto-kills all zombies on first breach
- **Shop System** - Unlock new plant types with earned coins
- **Save/Load** - Persistent progression across sessions
- **Night/Fog Rendering** - Dynamic overlays with plant light circles and gradient fog
- **Procedural Pixel Art** - All visuals rendered with Graphics primitives (no textures)
- **Particle Effects** - Explosions, pea splats, collection sparkles, death effects
- **Projectile Trails** - Visual trails for peas, snow peas, and fire peas
- **Chinese Localization** - Full Traditional Chinese UI text

## Building

### Prerequisites

- CMake 3.16+
- C++17 compiler
- [SF3 Engine](https://github.com/kevinlincg/sf3-engine) built as a sibling directory

### Desktop (macOS/Linux)

```bash
# Build SF3 engine first (sibling directory)
cd ../sf3-engine
cmake -B build -G Ninja -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build

# Build PVZ
cd ../pvz-game
cmake -B build -G Ninja -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build

# Run
./build/pvz
```

### Web (Emscripten)

```bash
# Install Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk && ./emsdk install latest && ./emsdk activate latest
source emsdk_env.sh

# Build SF3 for web
cd ../sf3-engine
emcmake cmake -B build-web -G Ninja \
    -DSF3_ENABLE_NET=OFF -DSF3_ENABLE_PACKAGE=OFF \
    -DSF3_ENABLE_GAMEPAD=OFF -DSF3_ENABLE_SPATIAL_AUDIO=OFF
cmake --build build-web

# Build PVZ for web
cd ../pvz-game
emcmake cmake -B build-web -G Ninja \
    -DSF3_BUILD_DIR=../sf3-engine/build-web \
    -DSF3_LIBRARY=../sf3-engine/build-web/libsf3.a
cmake --build build-web

# Output: build-web/pvz.html, pvz.js, pvz.wasm, pvz.data
```

### Source Mode (Full Debugging)

```bash
cmake -B build -G Ninja -DPVZ_SF3_SOURCE=ON -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

## Project Structure

```
pvz-game/
  CMakeLists.txt           # Build config (prebuilt + source modes, Emscripten support)
  src/
    main.cpp               # Entry point, scene registration, Emscripten main loop
    pvz_data.hpp           # Plant/Zombie stats, level definitions, grid constants
    pvz_components.hpp     # All ECS component structs
    pvz_systems.hpp/cpp    # ECS systems (attack, spawn, collision, boss AI, combos, drops)
    pvz_renderer.hpp/cpp   # Procedural pixel art for all game entities
    pvz_scenes.hpp/cpp     # Scene classes (Menu, LevelSelect, Shop, Game, GameOver)
    pvz_hud.hpp/cpp        # In-game HUD (seed packets, sun counter, wave progress)
    pvz_font.hpp/cpp       # Font manager (cross-platform, subset font for web)
  assets/
    fonts/
      pvz_font_subset.ttf  # 214KB subset font (228 CJK + Latin codepoints)
  web/
    shell.html             # Emscripten HTML shell template
```

## SF3 Engine Modules Used

| Module | Usage |
|--------|-------|
| **ECS** | All game entities (plants, zombies, projectiles, sun, drops) |
| **Scene** | Menu / LevelSelect / Shop / Game / GameOver flow |
| **Timer/Scheduler** | Wave timing, cooldowns, sun spawning |
| **Tween** | Sun/drop collection animations |
| **Physics** | AABB collision (projectile vs zombie, zombie vs plant) |
| **Particle** | Explosions, death effects, sparkles |
| **Camera** | Shake on explosions |
| **Font** | TrueType rendering for Chinese + English text |
| **Save** | Player progression persistence |
| **Profiler** | Debug overlay (F3 to toggle) |

## Deployment

The web build is deployed to Cloudflare Pages:

```bash
npx wrangler pages deploy build-web/deploy --project-name pvz-game
```

## License

This project is for educational and demonstration purposes.
