#include "../include/Game.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

static Game* gGame = nullptr;

extern "C" void frame() {
    if (!gGame->loop()) {
        emscripten_cancel_main_loop();
        gGame->cleanup();
    }
}
#endif

int main(int, char**) {
    Game game;
    if (!game.initialize()) {
        return 1;
    }

#ifdef __EMSCRIPTEN__
    gGame = &game;
    emscripten_set_main_loop(frame, 0, 1);
#else
    game.run();
    game.cleanup();
#endif

    return 0;
}