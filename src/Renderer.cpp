#include "../include/Renderer.hpp"
#include "../include/FontManager.hpp"
#include <iostream>
#include <cmath>
#include <vector>

Renderer::Renderer(SDL_Window* window) : window(window), renderer(nullptr) {}

Renderer::~Renderer() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
}

bool Renderer::initialize() {
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize font manager
    if (!FontManager::getInstance().initialize()) {
        std::cout << "Failed to initialize font manager!" << std::endl;
        return false;
    }

    return true;
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::renderBackground() {
    // Create gradient background
    for (int y = 0; y < GameConstants::SCREEN_HEIGHT; y++) {
        float t = static_cast<float>(y) / GameConstants::SCREEN_HEIGHT;

        Uint8 r = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.r * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.r * t);
        Uint8 g = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.g * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.g * t);
        Uint8 b = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.b * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.b * t);

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, 0, y, GameConstants::SCREEN_WIDTH, y);
    }
}

void Renderer::renderTarget(const Target& target) {
    float centerX = target.getX();
    float centerY = target.getY();
    float rotation = target.getRotation();

    // Draw wooden circles - improved rendering
    SDL_SetRenderDrawColor(renderer,
        GameConstants::Colors::WOOD.r,
        GameConstants::Colors::WOOD.g,
        GameConstants::Colors::WOOD.b, 255);

    // Draw filled circle with better algorithm
    int radius = static_cast<int>(GameConstants::TARGET_RADIUS);
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if ((w * w + h * h) <= (radius * radius)) {
                SDL_RenderPoint(renderer, centerX + w, centerY + h);
            }
        }
    }

    // Draw concentric rings
    SDL_SetRenderDrawColor(renderer,
        GameConstants::Colors::WOOD_DARK.r,
        GameConstants::Colors::WOOD_DARK.g,
        GameConstants::Colors::WOOD_DARK.b, 255);

    for (int ring = 20; ring < radius; ring += 20) {
        for (int angle = 0; angle < 360; angle += 2) {
            float radians = angle * M_PI / 180.0f;
            int x = centerX + ring * cos(radians);
            int y = centerY + ring * sin(radians);
            SDL_RenderPoint(renderer, x, y);
        }
    }

    // Draw center bullseye
    SDL_SetRenderDrawColor(renderer,
        GameConstants::Colors::RED.r,
        GameConstants::Colors::RED.g,
        GameConstants::Colors::RED.b, 255);

    for (int w = -10; w <= 10; w++) {
        for (int h = -10; h <= 10; h++) {
            if ((w * w + h * h) <= 100) {
                SDL_RenderPoint(renderer, centerX + w, centerY + h);
            }
        }
    }
}

void Renderer::renderKnife(const Knife& knife, bool useRotation) {
    if (!knife.isKnifeActive()) return;

    float x = knife.getX();
    float y = knife.getY();

    // FIXED: Render all knives the same way - simple upright knife
    // Handle
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_FRect handleRect = {
        x - GameConstants::KNIFE_WIDTH / 2,
        y + GameConstants::KNIFE_LENGTH / 3,
        static_cast<float>(GameConstants::KNIFE_WIDTH),
        static_cast<float>(GameConstants::KNIFE_LENGTH / 3)
    };
    SDL_RenderFillRect(renderer, &handleRect);

    // Blade
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
    SDL_FRect bladeRect = {
        x - GameConstants::KNIFE_WIDTH / 2,
        y - GameConstants::KNIFE_LENGTH / 3,
        static_cast<float>(GameConstants::KNIFE_WIDTH),
        static_cast<float>(GameConstants::KNIFE_LENGTH * 2 / 3)
    };
    SDL_RenderFillRect(renderer, &bladeRect);
}

void Renderer::renderKnives(const std::vector<Knife>& knives) {
    // Render all stuck knives the same way as flying knives
    for (const auto& knife : knives) {
        if (knife.isKnifeStuck()) {
            renderKnife(knife, false);  // FIXED: Don't use rotation, render normally
        }
    }
}

void Renderer::renderKnifeIndicators(int knivesLeft) {
    // Draw remaining knife indicators at the bottom left (rotated 45 degrees)
    float startX = 30.0f;
    float startY = GameConstants::KNIFE_INDICATOR_Y;

    for (int i = 0; i < knivesLeft; i++) {
        float knifeX = startX;
        float knifeY = startY - (i * GameConstants::KNIFE_INDICATOR_SPACING);

        // Draw small knife indicators rotated 45 degrees like original
        float scale = GameConstants::KNIFE_INDICATOR_SCALE;
        float width = GameConstants::KNIFE_WIDTH * scale;
        float length = GameConstants::KNIFE_LENGTH * scale;

        // 45 degree rotation
        float angle = 45.0f * M_PI / 180.0f;
        float cosA = cos(angle);
        float sinA = sin(angle);

        // Handle vertices (rotated)
        float handleLen = length / 3;
        float hx1 = (-width / 2) * cosA - (-handleLen / 2) * sinA;
        float hy1 = (-width / 2) * sinA + (-handleLen / 2) * cosA;
        float hx2 = (width / 2) * cosA - (-handleLen / 2) * sinA;
        float hy2 = (width / 2) * sinA + (-handleLen / 2) * cosA;
        float hx3 = (width / 2) * cosA - (handleLen / 2) * sinA;
        float hy3 = (width / 2) * sinA + (handleLen / 2) * cosA;
        float hx4 = (-width / 2) * cosA - (handleLen / 2) * sinA;
        float hy4 = (-width / 2) * sinA + (handleLen / 2) * cosA;

        // Draw handle
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_RenderLine(renderer, knifeX + hx1, knifeY + hy1, knifeX + hx2, knifeY + hy2);
        SDL_RenderLine(renderer, knifeX + hx2, knifeY + hy2, knifeX + hx3, knifeY + hy3);
        SDL_RenderLine(renderer, knifeX + hx3, knifeY + hy3, knifeX + hx4, knifeY + hy4);
        SDL_RenderLine(renderer, knifeX + hx4, knifeY + hy4, knifeX + hx1, knifeY + hy1);

        // Blade vertices (rotated)
        float bladeLen = length * 2 / 3;
        float bx1 = (-width / 4) * cosA - (handleLen / 2) * sinA;
        float by1 = (-width / 4) * sinA + (handleLen / 2) * cosA;
        float bx2 = (width / 4) * cosA - (handleLen / 2) * sinA;
        float by2 = (width / 4) * sinA + (handleLen / 2) * cosA;
        float bx3 = 0 * cosA - (handleLen / 2 + bladeLen) * sinA;
        float by3 = 0 * sinA + (handleLen / 2 + bladeLen) * cosA;

        // Draw blade
        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        SDL_RenderLine(renderer, knifeX + bx1, knifeY + by1, knifeX + bx2, knifeY + by2);
        SDL_RenderLine(renderer, knifeX + bx2, knifeY + by2, knifeX + bx3, knifeY + by3);
        SDL_RenderLine(renderer, knifeX + bx3, knifeY + by3, knifeX + bx1, knifeY + by1);
    }
}

void Renderer::renderText(const std::string& text, int x, int y,
    const SDL_Color& color, bool centered, const std::string& fontName) {
    TTF_Font* font = FontManager::getInstance().getFont(fontName);
    if (!font) {
        std::cerr << "Font not found: " << fontName << std::endl;
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
    if (!surface) {
        std::cerr << "Failed to create text surface" << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };

    if (centered) {
        dstRect.x -= surface->w / 2.0f;
        dstRect.y -= surface->h / 2.0f;
    }

    SDL_DestroySurface(surface);
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
}

void Renderer::renderMenu() {
    clear();
    renderBackground();

    // Title with shadow effect
    SDL_Color shadowColor = { 0, 0, 0, 128 };
    SDL_Color titleColor = { 255, 255, 255, 255 };

    renderText("KNIFE", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 3 - 40 + 4,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("KNIFE", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 3 - 40,
        titleColor, true, FontManager::TITLE_FONT);

    renderText("HIT", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 3 + 40 + 4,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("HIT", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 3 + 40,
        titleColor, true, FontManager::TITLE_FONT);

    // Pulsing effect for TAP TO PLAY
    float pulse = (sin(SDL_GetTicks() / 300.0f) + 1.0f) / 2.0f;
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    SDL_Color tapColor = { 255, 255, 255, alpha };

    renderText("TAP TO PLAY", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT * 2 / 3,
        tapColor, true, FontManager::UI_FONT);

    present();
}

void Renderer::renderHUD(int level, int score) {
    // Stage indicator
    std::string stageText = "STAGE " + std::to_string(level);
    renderText(stageText, 80, 40,
        { 255, 255, 255, 255 }, true, FontManager::UI_FONT);

    // Score number (removed apple)
    renderText(std::to_string(score),
        GameConstants::SCREEN_WIDTH - 100, 40,
        { 255, 255, 255, 255 }, true, FontManager::SCORE_FONT);
}

void Renderer::renderGameOver(int score) {
    clear();
    renderBackground();

    // Red tinted overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
    SDL_RenderFillRect(renderer, nullptr);

    SDL_Color shadowColor = { 0, 0, 0, 200 };
    SDL_Color gameOverColor = { 255, 50, 50, 255 };

    renderText("GAME OVER", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 - 100 + 4,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("GAME OVER", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 - 100,
        gameOverColor, true, FontManager::TITLE_FONT);

    std::string scoreText = "SCORE: " + std::to_string(score);
    renderText(scoreText, GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 - 20,
        { 255, 255, 255, 255 }, true, FontManager::UI_FONT);

    // Pulsing restart button
    float pulse = (sin(SDL_GetTicks() / 300.0f) + 1.0f) / 2.0f;
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    SDL_Color restartColor = { 255, 255, 255, alpha };

    renderText("TAP TO RESTART", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 + 60,
        restartColor, true, FontManager::UI_FONT);

    present();
}

void Renderer::renderLevelComplete() {
    clear();
    renderBackground();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 76, 217, 100, 30);
    SDL_RenderFillRect(renderer, nullptr);

    SDL_Color shadowColor = { 0, 0, 0, 200 };
    SDL_Color successColor = { 76, 217, 100, 255 };

    renderText("STAGE", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 - 50 + 4,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("STAGE", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 - 50,
        successColor, true, FontManager::TITLE_FONT);

    renderText("COMPLETE!", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 + 20 + 4,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("COMPLETE!", GameConstants::SCREEN_WIDTH / 2,
        GameConstants::SCREEN_HEIGHT / 2 + 20,
        successColor, true, FontManager::TITLE_FONT);

    present();
}

void Renderer::renderGame(const Target& target, const std::vector<Knife>& knives,
    const Knife& currentKnife, int level, int score, int knivesLeft) {
    clear();
    renderBackground();

    // Render target (rotating)
    renderTarget(target);

    // Render stuck knives (rotating with target)
    renderKnives(knives);

    // Render current flying knife
    if (currentKnife.isKnifeActive() && !currentKnife.isKnifeStuck()) {
        renderKnife(currentKnife, false);  // No rotation for flying knife
    }

    // Render HUD
    renderHUD(level, score);

    // Render knife indicators (remaining knives)
    renderKnifeIndicators(knivesLeft);

    present();
}