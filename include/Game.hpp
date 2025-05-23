#pragma once

#include <SDL3/SDL.h>
#include "Knife.hpp"
#include "Target.hpp"
#include "Renderer.hpp"

enum class GameState {
    MENU,
    PLAYING,
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
    
    SDL_Window* window;
    Renderer* renderer;
    GameState currentState;
    
    Target target;
    Knife currentKnife;
    
    int level;
    int knivesLeft;
    int score;
    bool canThrow;
    
    Uint64 lastTime;
    float gameTime;
}; 