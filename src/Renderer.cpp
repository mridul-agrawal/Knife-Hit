#include "../include/Renderer.hpp"
#include <iostream>
#include <cmath>

Renderer::Renderer(SDL_Window* window)
    : window(window)
    , renderer(nullptr)
    , titleFont(nullptr)
    , uiFont(nullptr)
    , numberFont(nullptr) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Enable blending for smooth graphics
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Initialize TTF
    if (!TTF_Init()) {
        std::cout << "TTF_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load fonts with better sizes
    titleFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 48);  // Bigger title
    if (!titleFont) {
        std::cout << "Warning: Could not load title font" << std::endl;
    }

    uiFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);  // Bigger UI
    if (!uiFont) {
        std::cout << "Warning: Could not load UI font" << std::endl;
    }

    numberFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 32);  // Bigger numbers
    if (!numberFont) {
        std::cout << "Warning: Could not load number font" << std::endl;
    }

    return true;
}

void Renderer::cleanup() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (uiFont) {
        TTF_CloseFont(uiFont);
        uiFont = nullptr;
    }
    if (numberFont) {
        TTF_CloseFont(numberFont);
        numberFont = nullptr;
    }

    TTF_Quit();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}

void Renderer::setColor(const GameConstants::Colors::Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Renderer::drawGradientBackground() {
    // Draw gradient background like original game
    for (int y = 0; y < GameConstants::SCREEN_HEIGHT; y++) {
        float ratio = (float)y / GameConstants::SCREEN_HEIGHT;

        // Interpolate between top and bottom colors
        unsigned char r = GameConstants::Colors::BACKGROUND_TOP.r * (1 - ratio) +
            GameConstants::Colors::BACKGROUND_DARK.r * ratio;
        unsigned char g = GameConstants::Colors::BACKGROUND_TOP.g * (1 - ratio) +
            GameConstants::Colors::BACKGROUND_DARK.g * ratio;
        unsigned char b = GameConstants::Colors::BACKGROUND_TOP.b * (1 - ratio) +
            GameConstants::Colors::BACKGROUND_DARK.b * ratio;

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, 0, y, GameConstants::SCREEN_WIDTH, y);
    }
}

void Renderer::drawKnife(float x, float y, float angle, float scale) {
    float knifeWidth = GameConstants::KNIFE_WIDTH * scale;
    float knifeLength = GameConstants::KNIFE_LENGTH * scale;

    // Save render state
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw knife handle (bottom part)
    setColor(GameConstants::Colors::KNIFE_HANDLE);
    SDL_FRect handleRect = {
        x - knifeWidth / 2,
        y - knifeLength * 0.35f,
        knifeWidth,
        knifeLength * 0.35f
    };
    SDL_RenderFillRect(renderer, &handleRect);

    // Draw handle detail
    setColor(GameConstants::Colors::WOOD_DARK);
    SDL_FRect handleDetail = {
        x - knifeWidth / 2 + 2,
        y - knifeLength * 0.2f,
        knifeWidth - 4,
        2
    };
    SDL_RenderFillRect(renderer, &handleDetail);

    // Draw knife blade
    setColor(GameConstants::Colors::KNIFE_BLADE);
    SDL_FRect bladeRect = {
        x - knifeWidth / 2 + 1,
        y - knifeLength,
        knifeWidth - 2,
        knifeLength * 0.65f
    };
    SDL_RenderFillRect(renderer, &bladeRect);

    // Draw blade edge (darker line)
    setColor(GameConstants::Colors::KNIFE_BLADE_EDGE);
    SDL_RenderLine(renderer,
        x - knifeWidth / 2 + 1, y - knifeLength,
        x - knifeWidth / 2 + 1, y - knifeLength * 0.35f);
    SDL_RenderLine(renderer,
        x + knifeWidth / 2 - 1, y - knifeLength,
        x + knifeWidth / 2 - 1, y - knifeLength * 0.35f);

    // Draw knife tip (triangle)
    SDL_FPoint tip[3] = {
        {x - knifeWidth / 2 + 1, y - knifeLength},
        {x + knifeWidth / 2 - 1, y - knifeLength},
        {x, y - knifeLength - 8 * scale}
    };

    // Note: SDL3 doesn't have RenderFillTriangle, so we draw lines
    setColor(GameConstants::Colors::KNIFE_BLADE);
    for (int i = 0; i < 3; i++) {
        SDL_RenderLine(renderer, tip[i].x, tip[i].y, tip[(i + 1) % 3].x, tip[(i + 1) % 3].y);
    }
}

void Renderer::drawTarget(const Target& target) {
    const float x = target.getX();
    const float y = target.getY();
    const float radius = target.getRadius();
    const float rotation = target.getRotation();

    // Draw wood texture circles (multiple layers for depth)
    const int layers = 5;
    for (int layer = layers; layer > 0; layer--) {
        float layerRadius = radius * (0.9f + 0.1f * layer / layers);

        // Alternate wood colors for rings
        if (layer % 2 == 0) {
            setColor(GameConstants::Colors::WOOD);
        }
        else {
            setColor(GameConstants::Colors::WOOD_LIGHT);
        }

        // Draw filled circle
        const int segments = 60;
        for (int i = 0; i < segments; i++) {
            float angle1 = (float)i / segments * 2 * M_PI;
            float angle2 = (float)(i + 1) / segments * 2 * M_PI;

            // Draw triangle from center to edge
            SDL_FPoint triangle[3] = {
                {x, y},
                {x + cos(angle1) * layerRadius, y + sin(angle1) * layerRadius},
                {x + cos(angle2) * layerRadius, y + sin(angle2) * layerRadius}
            };

            // Draw lines to form triangle
            for (int j = 0; j < 3; j++) {
                SDL_RenderLine(renderer,
                    triangle[j].x, triangle[j].y,
                    triangle[(j + 1) % 3].x, triangle[(j + 1) % 3].y);
            }
        }
    }

    // Draw wood grain lines
    setColor(GameConstants::Colors::WOOD_RING);
    for (int i = 0; i < 8; i++) {
        float angle = (rotation + i * 45) * M_PI / 180;
        float startRadius = radius * 0.2f;
        float endRadius = radius * 0.9f;

        SDL_RenderLine(renderer,
            x + cos(angle) * startRadius,
            y + sin(angle) * startRadius,
            x + cos(angle) * endRadius,
            y + sin(angle) * endRadius);
    }

    // Draw center bullseye
    setColor(GameConstants::Colors::RED);
    const int centerSize = 8;
    SDL_FRect center = { x - centerSize / 2, y - centerSize / 2, centerSize, centerSize };
    SDL_RenderFillRect(renderer, &center);

    // Draw white dot in center
    setColor(GameConstants::Colors::WHITE);
    SDL_FRect centerDot = { x - 2, y - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &centerDot);

    // Draw stuck knives
    const auto& angles = target.getStuckKnifeAngles();
    const auto& distances = target.getStuckKnifeDistances();
    for (size_t i = 0; i < angles.size(); i++) {
        float knifeAngle = angles[i] + rotation;
        float knifeX = x + cos(knifeAngle * M_PI / 180) * distances[i];
        float knifeY = y + sin(knifeAngle * M_PI / 180) * distances[i];
        drawKnife(knifeX, knifeY, knifeAngle + 90);
    }
}

void Renderer::renderMenu() {
    // Draw gradient background
    drawGradientBackground();

    int centerX = GameConstants::SCREEN_WIDTH / 2;
    int centerY = GameConstants::SCREEN_HEIGHT / 2;

    // Draw title
    drawTTFTextCentered("KNIFE", centerX, centerY - 250, titleFont, GameConstants::Colors::WHITE);
    drawTTFTextCentered("HIT", centerX, centerY - 200, titleFont, GameConstants::Colors::YELLOW);

    // Draw sample target
    Target sampleTarget;
    sampleTarget.setPosition(centerX, centerY - 50);
    drawTarget(sampleTarget);

    // Draw "Tap to play" message
    drawTTFTextCentered("TAP TO PLAY", centerX, centerY + 120, uiFont, GameConstants::Colors::WHITE);

    // Draw sample knife at bottom
    drawKnife(centerX, GameConstants::KNIFE_START_Y);

    SDL_RenderPresent(renderer);
}

void Renderer::renderGame(const Target& target, const Knife& currentKnife, int knivesLeft, int score, int level) {
    // Draw gradient background
    drawGradientBackground();

    // Draw target
    drawTarget(target);

    // Draw current knife
    if (currentKnife.isKnifeActive()) {
        drawKnife(currentKnife.getX(), currentKnife.getY());
    }

    // Draw score at top center
    int centerX = GameConstants::SCREEN_WIDTH / 2;
    drawTTFTextCentered(std::to_string(score), centerX, GameConstants::UI_TOP_MARGIN, numberFont, GameConstants::Colors::WHITE);

    // Draw apple icon next to score (placeholder - just draw a circle)
    setColor(GameConstants::Colors::RED);
    SDL_FRect apple = { centerX + 40, GameConstants::UI_TOP_MARGIN + 10, 20, 20 };
    SDL_RenderFillRect(renderer, &apple);

    // Draw level indicator at top left
    drawTTFText("STAGE " + std::to_string(level), GameConstants::UI_MARGIN, GameConstants::UI_TOP_MARGIN, uiFont, GameConstants::Colors::WHITE);

    // Draw knife indicators at bottom
    float indicatorStartX = centerX - (knivesLeft * GameConstants::KNIFE_INDICATOR_SPACING / 2);
    for (int i = 0; i < knivesLeft; i++) {
        float x = indicatorStartX + i * GameConstants::KNIFE_INDICATOR_SPACING;
        drawKnife(x, GameConstants::KNIFE_INDICATOR_Y, 0, GameConstants::KNIFE_INDICATOR_SCALE);
    }

    SDL_RenderPresent(renderer);
}

void Renderer::renderGameOver(int score) {
    drawGradientBackground();

    int centerX = GameConstants::SCREEN_WIDTH / 2;
    int centerY = GameConstants::SCREEN_HEIGHT / 2;

    drawTTFTextCentered("GAME OVER", centerX, centerY - 100, titleFont, GameConstants::Colors::RED);
    drawTTFTextCentered("SCORE: " + std::to_string(score), centerX, centerY, uiFont, GameConstants::Colors::WHITE);
    drawTTFTextCentered("TAP TO RESTART", centerX, centerY + 100, uiFont, GameConstants::Colors::WHITE);

    SDL_RenderPresent(renderer);
}

void Renderer::renderLevelComplete(int level, int score) {
    drawGradientBackground();

    int centerX = GameConstants::SCREEN_WIDTH / 2;
    int centerY = GameConstants::SCREEN_HEIGHT / 2;

    drawTTFTextCentered("STAGE COMPLETE!", centerX, centerY - 100, titleFont, GameConstants::Colors::GREEN);
    drawTTFTextCentered("STAGE " + std::to_string(level), centerX, centerY, uiFont, GameConstants::Colors::WHITE);
    drawTTFTextCentered("SCORE: " + std::to_string(score), centerX, centerY + 40, uiFont, GameConstants::Colors::YELLOW);
    drawTTFTextCentered("TAP TO CONTINUE", centerX, centerY + 120, uiFont, GameConstants::Colors::WHITE);

    SDL_RenderPresent(renderer);
}

void Renderer::drawTTFText(const std::string& text, int x, int y, TTF_Font* font, const GameConstants::Colors::Color& color) {
    if (!font) return;

    SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), 0, sdlColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) return;

    float width, height;
    SDL_GetTextureSize(texture, &width, &height);
    SDL_FRect destRect = { (float)x, (float)y, width, height };

    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
    SDL_DestroyTexture(texture);
}

void Renderer::drawTTFTextCentered(const std::string& text, int centerX, int y, TTF_Font* font, const GameConstants::Colors::Color& color) {
    if (!font) return;

    SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), 0, sdlColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) return;

    float width, height;
    SDL_GetTextureSize(texture, &width, &height);
    SDL_FRect destRect = { (float)(centerX - width / 2), (float)y, width, height };

    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
    SDL_DestroyTexture(texture);
}