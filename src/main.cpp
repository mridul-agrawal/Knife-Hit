#include "../include/Game.hpp"
#include <iostream>
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

int main(int argc, char** argv) {
    Game game;

    // Default dimensions (can be changed by external systems)
    float width = 1080.0f;   // Default reference width
    float height = 1920.0f;  // Default reference height

    // Example: Parse command line arguments for custom dimensions
    if (argc >= 3) {
        try {
            width = std::stof(argv[1]);
            height = std::stof(argv[2]);
            std::cout << "Using custom dimensions from command line: "
                << width << "x" << height << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Invalid dimensions provided, using defaults" << std::endl;
        }
    }

    // Example: Different preset dimensions for testing
    // Uncomment any of these to test different screen sizes:

    // Phone portrait (9:16 aspect ratio)
    width = 225; height = 400;

    // Tablet portrait (3:4 aspect ratio)
    // width = 768; height = 1024;

    // Desktop/laptop (16:9 aspect ratio)
    // width = 1280; height = 720;

    // Ultra-wide (21:9 aspect ratio)
    //  width = 1800; height = 822;

    std::cout << "Initializing game with dimensions: " << width << "x" << height << std::endl;

    if (!game.initialize(width, height)) {
        std::cout << "Failed to initialize game!" << std::endl;
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

// Example function that external systems can call to change dimensions
extern "C" void setGameDimensions(float width, float height) {
    // This could be called by web JavaScript, mobile frameworks, etc.
    Game* game = nullptr; // Get game instance from somewhere
    if (game) {
        game->setScreenDimensions(width, height);
    }
}