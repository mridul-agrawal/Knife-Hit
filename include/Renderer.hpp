#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "Knife.hpp"
#include "Target.hpp"

class Renderer {
public:
    Renderer(SDL_Window* window);
    ~Renderer();
    
    bool initialize();
    void cleanup();
    
    void renderMenu();
    void renderGame(const Target& target, const Knife& currentKnife, int knivesLeft, int score, int level);
    void renderGameOver(int score);
    void renderLevelComplete(int level, int score);
    
private:
    void drawKnife(float x, float y, float angle = 0);
    void drawTarget(const Target& target);
    void drawText(const std::string& text, int x, int y);
    void drawNumber(int num, int x, int y);
    void drawCharacter(char c, int x, int y);
    
    SDL_Window* window;
    SDL_Renderer* renderer;
}; 