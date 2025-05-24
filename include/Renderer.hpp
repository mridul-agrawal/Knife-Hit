#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>
#include "GameConstants.hpp"
#include "Target.hpp"
#include "Knife.hpp"

class Renderer {
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    bool initialize();
    void clear();
    void present();

    // Rendering methods
    void renderBackground();
    void renderTarget(const Target& target);
    void renderKnife(const Knife& knife, bool useRotation = false);  // UPDATED: Added rotation param
    void renderKnives(const std::vector<Knife>& knives);
    void renderHUD(int level, int score);
    void renderKnifeIndicators(int knivesLeft);  // NEW: Show remaining knives
    void renderMenu();
    void renderGameOver(int score);
    void renderLevelComplete();
    void renderGame(const Target& target, const std::vector<Knife>& knives,
        const Knife& currentKnife, int level, int score, int knivesLeft);  // UPDATED: Added knivesLeft

    // Text rendering with font support
    void renderText(const std::string& text, int x, int y,
        const SDL_Color& color, bool centered = false,
        const std::string& fontName = "ui");
    // Add new method declaration:
    void renderCollisionPause(const Target& target, const std::vector<Knife>& knives,
        const Knife& currentKnife, int level, int score, int knivesLeft);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;  // Background texture
    SDL_Texture* targetTexture;      // NEW: Target texture
    SDL_Texture* knifeTexture;      // NEW: Knife texture

    // Helper methods
    bool loadBackgroundTexture();
    void cleanupBackgroundTexture();
    bool loadTargetTexture();        // NEW: Load target texture
    void cleanupTargetTexture();     // NEW: Cleanup target texture
    bool loadKnifeTexture();        // NEW: Load knife texture
    void cleanupKnifeTexture();     // NEW: Cleanup knife texture
};