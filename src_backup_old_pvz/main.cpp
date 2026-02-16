#include "sf3.hpp"
#include "scene/sf3_scene.hpp"
#include "performance/sf3_profiler.hpp"
#include "pvz_scenes.hpp"
#include "pvz_data.hpp"
#include "pvz_font.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace SF3;

static ProfilerOverlay* s_profiler = nullptr;

static void mainLoop() {
    auto& app = App::instance();
    auto& sm = SceneManager::instance();

    app.pollEvents();

    if (Input::keyPressed(Key::F3) && s_profiler) {
        s_profiler->toggle();
    }

    float dt = app.deltaTime();
    sm.update(dt);
    if (s_profiler) s_profiler->update(dt);

    Graphics::beginFrame();
    Graphics::clear(Color::Black());
    sm.render();
    if (s_profiler) s_profiler->render();
    Graphics::endFrame();
}

int main(int, char**) {
    auto& app = App::instance();
    Config config;
    config.title = "Plants vs Zombies - SF3 Engine";
    config.width = static_cast<int>(PVZ::SCREEN_W);
    config.height = static_cast<int>(PVZ::SCREEN_H);
    config.vsync = true;

    if (failed(app.init(config))) return 1;

    // Load saved progress
    PVZ::loadProgress();

    // Initialize font system
    PVZ::FontManager::instance().init();

    // Profiler overlay (F3 to toggle)
    static ProfilerOverlay profiler;
    s_profiler = &profiler;

    // Register scenes
    auto& sm = SceneManager::instance();
    sm.registerScene<PVZ::MenuScene>("menu");
    sm.registerScene<PVZ::LevelSelectScene>("levelselect");
    sm.registerScene<PVZ::ShopScene>("shop");
    sm.registerScene<PVZ::GameScene>("game");
    sm.registerScene<PVZ::GameOverScene>("gameover");

    sm.switchTo("menu", SceneTransition::Fade(0.5f));

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (app.running()) {
        mainLoop();
    }
#endif

    return 0;
}
