#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <SDL3/SDL.h>
#include <vector>
#include "Knife.hpp"
#include "Target.hpp"
#include "Renderer.hpp"

enum class GameState {
    MENU,
    PLAYING,
    COLLISION_PAUSE,
    GAME_OVER,
    LEVEL_COMPLETE
};

class Game {
public:
    Game();
    ~Game();

    // Initialize with default dimensions
    bool initialize();

    // Initialize with custom dimensions (for external calls)
    bool initialize(float width, float height);

    // Set screen dimensions (can be called by external systems)
    void setScreenDimensions(float width, float height);

    void run();
    void cleanup();
    bool loop();

private:
    void handleEvents();
    void update(float deltaTime);
    void handleInput();
    void throwKnife();
    bool checkKnifeCollision();
    void initializeLevel();
    void updateStuckKnives();

    // Internal initialization after dimensions are set
    bool initializeInternal();

    SDL_Window* window;
    Renderer* renderer;
    GameState currentState;

    Target target;
    Knife currentKnife;
    std::vector<Knife> stuckKnives;

    int level;
    int knivesLeft;
    int score;
    bool canThrow;
    bool shouldExit;

    bool collisionDetected;
    float collisionPauseTimer;
    static constexpr float COLLISION_PAUSE_DURATION = 2.5f;

    Uint64 lastTime;
    float gameTime;

    // Screen dimensions
    float screenWidth;
    float screenHeight;
    bool dimensionsSet;
};