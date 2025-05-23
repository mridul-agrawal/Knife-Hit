#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "Knife.hpp"
#include "Target.hpp"
#include "GameConstants.hpp"

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
    void setColor(const GameConstants::Colors::Color& color);

    void drawTTFText(const std::string& text, int x, int y, TTF_Font* font, const GameConstants::Colors::Color& color = GameConstants::Colors::WHITE);
    void drawTTFTextCentered(const std::string& text, int centerX, int y, TTF_Font* font, const GameConstants::Colors::Color& color = GameConstants::Colors::WHITE);

    SDL_Window* window;
    SDL_Renderer* renderer;

    TTF_Font* titleFont;   // For big text like "KNIFE HIT"
    TTF_Font* uiFont;      // For UI text like "LEVEL:", "SCORE:"
    TTF_Font* numberFont;  // For numbers
};