#pragma once

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

    bool initialize();
    void run();
    void cleanup();

private:
    void handleEvents();
    void update(float deltaTime);
    void handleInput();
    void throwKnife();
    bool checkKnifeCollision();
    void initializeLevel();
    void updateStuckKnives();  // NEW: Update positions of stuck knives

    SDL_Window* window;
    Renderer* renderer;
    GameState currentState;

    Target target;
    Knife currentKnife;
    std::vector<Knife> stuckKnives;  // NEW: Store stuck knives

    int level;
    int knivesLeft;
    int score;
    bool canThrow;

    bool collisionDetected;
    float collisionPauseTimer;
    static constexpr float COLLISION_PAUSE_DURATION = 2.5f;

    Uint64 lastTime;
    float gameTime;
};