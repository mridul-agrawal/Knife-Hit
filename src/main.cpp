#include "../include/Game.hpp"
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

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
    float width = 225;   // Default reference width
    float height = 400;  // Default reference height

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

#ifdef __EMSCRIPTEN__
    double widthD = width, heightD = height;


    // The selector must include '#' when referring to an element by ID
    EMSCRIPTEN_RESULT result = emscripten_get_element_css_size("#myCanvas", &widthD, &heightD);
    if (result == EMSCRIPTEN_RESULT_SUCCESS) {
        width = static_cast<float>(widthD);
        height = static_cast<float>(heightD);
        std::cout << "Canvas CSS size: " << width << " x " << height << "\n";
    }
    else {
        std::cerr << "Failed to get CSS size of #myCanvas\n";
    }
#endif

    // Example: Different preset dimensions for testing
    // Uncomment any of these to test different screen sizes:

    // Phone portrait (9:16 aspect ratio)
    // width = 225; height = 400;

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