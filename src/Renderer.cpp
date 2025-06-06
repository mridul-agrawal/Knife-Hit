#include "../include/Renderer.hpp"
#include "../include/FontManager.hpp"
#include <iostream>
#include <cmath>
#include <vector>

Renderer::Renderer(SDL_Window* window) : window(window), renderer(nullptr),
backgroundTexture(nullptr), targetTexture(nullptr), knifeTexture(nullptr) {}

Renderer::~Renderer() {
    cleanupBackgroundTexture();
    cleanupTargetTexture();
    cleanupKnifeTexture();
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

    // Load background texture
    if (!loadBackgroundTexture()) {
        std::cout << "Failed to load background texture!" << std::endl;
        return false;
    }

    // Load target texture
    if (!loadTargetTexture()) {
        std::cout << "Failed to load target texture!" << std::endl;
        return false;
    }

    // Load knife texture
    if (!loadKnifeTexture()) {
        std::cout << "Failed to load knife texture!" << std::endl;
        return false;
    }

    return true;
}

bool Renderer::loadBackgroundTexture() {
    SDL_Surface* surface = SDL_LoadBMP("assets/images/background.bmp");
    if (!surface) {
        surface = SDL_LoadBMP("background.bmp");
    }

    if (!surface) {
        std::cout << "Failed to load background image: " << SDL_GetError() << std::endl;
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!backgroundTexture) {
        std::cout << "Failed to create background texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool Renderer::loadTargetTexture() {
    SDL_Surface* surface = SDL_LoadBMP("assets/images/target.bmp");
    if (!surface) {
        surface = SDL_LoadBMP("target.bmp");
    }

    if (!surface) {
        std::cout << "Failed to load target image: " << SDL_GetError() << std::endl;
        return false;
    }

    targetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!targetTexture) {
        std::cout << "Failed to create target texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool Renderer::loadKnifeTexture() {
    SDL_Surface* surface = SDL_LoadBMP("assets/images/knife.bmp");
    if (!surface) {
        surface = SDL_LoadBMP("knife.bmp");
    }

    if (!surface) {
        std::cout << "Failed to load knife image: " << SDL_GetError() << std::endl;
        return false;
    }

    knifeTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!knifeTexture) {
        std::cout << "Failed to create knife texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Renderer::cleanupKnifeTexture() {
    if (knifeTexture) {
        SDL_DestroyTexture(knifeTexture);
        knifeTexture = nullptr;
    }
}

void Renderer::cleanupBackgroundTexture() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void Renderer::cleanupTargetTexture() {
    if (targetTexture) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::renderBackground() {
    if (backgroundTexture) {
        // Scale and render the background image to fit the screen
        SDL_FRect destRect = {
            0, 0,
            GameConstants::CURRENT_WIDTH,
            GameConstants::CURRENT_HEIGHT
        };
        SDL_RenderTexture(renderer, backgroundTexture, nullptr, &destRect);
    }
    else {
        // Fallback to gradient background if image fails to load
        for (int y = 0; y < static_cast<int>(GameConstants::CURRENT_HEIGHT); y++) {
            float t = static_cast<float>(y) / GameConstants::CURRENT_HEIGHT;

            Uint8 r = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.r * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.r * t);
            Uint8 g = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.g * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.g * t);
            Uint8 b = static_cast<Uint8>(GameConstants::Colors::BACKGROUND_TOP.b * (1 - t) + GameConstants::Colors::BACKGROUND_BOTTOM.b * t);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderLine(renderer, 0, y, static_cast<int>(GameConstants::CURRENT_WIDTH), y);
        }
    }
}

void Renderer::renderTarget(const Target& target) {
    float centerX = target.getX();
    float centerY = target.getY();
    float rotation = target.getRotation();
    float radius = target.getRadius();

    if (targetTexture) {
        // Calculate the destination rectangle (centered on target position)
        float diameter = radius * 2.0f;
        SDL_FRect destRect = {
            centerX - radius,
            centerY - radius,
            diameter,
            diameter
        };

        // Create rotation point (center of the target)
        SDL_FPoint center = {
            radius,
            radius
        };

        // Render the rotated target image
        SDL_RenderTextureRotated(renderer, targetTexture, nullptr, &destRect,
            rotation, &center, SDL_FLIP_NONE);
    }
    else {
        // Fallback to original geometric rendering if texture fails to load
        SDL_SetRenderDrawColor(renderer,
            GameConstants::Colors::WOOD.r,
            GameConstants::Colors::WOOD.g,
            GameConstants::Colors::WOOD.b, 255);

        // Draw filled circle with better algorithm
        int radiusInt = static_cast<int>(radius);
        for (int w = -radiusInt; w <= radiusInt; w++) {
            for (int h = -radiusInt; h <= radiusInt; h++) {
                if ((w * w + h * h) <= (radiusInt * radiusInt)) {
                    SDL_RenderPoint(renderer, centerX + w, centerY + h);
                }
            }
        }

        // Draw concentric rings (scaled)
        SDL_SetRenderDrawColor(renderer,
            GameConstants::Colors::WOOD_DARK.r,
            GameConstants::Colors::WOOD_DARK.g,
            GameConstants::Colors::WOOD_DARK.b, 255);

        int ringSpacing = static_cast<int>(GameConstants::scaleUniform(20));
        for (int ring = ringSpacing; ring < radiusInt; ring += ringSpacing) {
            for (int angle = 0; angle < 360; angle += 2) {
                float radians = angle * M_PI / 180.0f;
                int x = centerX + ring * cos(radians);
                int y = centerY + ring * sin(radians);
                SDL_RenderPoint(renderer, x, y);
            }
        }

        // Draw center bullseye (scaled)
        SDL_SetRenderDrawColor(renderer,
            GameConstants::Colors::RED.r,
            GameConstants::Colors::RED.g,
            GameConstants::Colors::RED.b, 255);

        int bullseyeRadius = static_cast<int>(GameConstants::scaleUniform(10));
        for (int w = -bullseyeRadius; w <= bullseyeRadius; w++) {
            for (int h = -bullseyeRadius; h <= bullseyeRadius; h++) {
                if ((w * w + h * h) <= (bullseyeRadius * bullseyeRadius)) {
                    SDL_RenderPoint(renderer, centerX + w, centerY + h);
                }
            }
        }
    }
}

void Renderer::renderKnife(const Knife& knife, bool useRotation) {
    if (!knife.isKnifeActive()) return;

    float x = knife.getX();
    float y = knife.getY();

    if (knifeTexture) {
        // Use knife image with dynamic scaling
        float knifeWidth = GameConstants::getKnifeWidth();
        float knifeLength = GameConstants::getKnifeLength();

        SDL_FRect destRect = {
            x - knifeWidth / 2,
            y - knifeLength / 2,
            knifeWidth,
            knifeLength
        };

        if (useRotation && knife.isKnifeStuck()) {
            // Render rotated stuck knife
            SDL_FPoint center = {
                knifeWidth / 2,
                knifeLength / 2
            };
            SDL_RenderTextureRotated(renderer, knifeTexture, nullptr, &destRect,
                knife.getRotation(), &center, SDL_FLIP_NONE);
        }
        else {
            // Render normal upright knife
            SDL_RenderTexture(renderer, knifeTexture, nullptr, &destRect);
        }
    }
    else {
        // Fallback to geometric rendering with dynamic scaling
        float knifeWidth = GameConstants::getKnifeWidth();
        float knifeLength = GameConstants::getKnifeLength();

        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_FRect handleRect = {
            x - knifeWidth / 2,
            y + knifeLength / 3,
            knifeWidth,
            knifeLength / 3
        };
        SDL_RenderFillRect(renderer, &handleRect);

        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        SDL_FRect bladeRect = {
            x - knifeWidth / 2,
            y - knifeLength / 3,
            knifeWidth,
            knifeLength * 2 / 3
        };
        SDL_RenderFillRect(renderer, &bladeRect);
    }
}

void Renderer::renderKnives(const std::vector<Knife>& knives) {
    for (const auto& knife : knives) {
        if (knife.isKnifeStuck()) {
            renderKnife(knife, true);
        }
    }
}

void Renderer::renderKnifeIndicators(int knivesLeft) {
    if (!knifeTexture) {
        // Fallback to geometric indicators with scaling
        float startX = GameConstants::getUIMargin();
        float startY = GameConstants::getKnifeIndicatorY();
        float spacing = GameConstants::getKnifeIndicatorSpacing();
        float knifeWidth = GameConstants::getKnifeWidth() * GameConstants::KNIFE_INDICATOR_SCALE;
        float knifeLength = GameConstants::getKnifeLength() * GameConstants::KNIFE_INDICATOR_SCALE;

        for (int i = 0; i < knivesLeft; i++) {
            float knifeX = startX;
            float knifeY = startY - (i * spacing);

            // Draw handle
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
            SDL_FRect handleRect = {
                knifeX - knifeWidth / 2,
                knifeY + knifeLength / 3,
                knifeWidth,
                knifeLength / 3
            };
            SDL_RenderFillRect(renderer, &handleRect);

            // Draw blade
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
            SDL_FRect bladeRect = {
                knifeX - knifeWidth / 2,
                knifeY - knifeLength / 3,
                knifeWidth,
                knifeLength * 2 / 3
            };
            SDL_RenderFillRect(renderer, &bladeRect);
        }
        return;
    }

    // Use knife image for indicators with dynamic scaling
    float startX = GameConstants::getUIMargin();
    float startY = GameConstants::getKnifeIndicatorY();
    float spacing = GameConstants::getKnifeIndicatorSpacing();
    float scale = GameConstants::KNIFE_INDICATOR_SCALE;

    for (int i = 0; i < knivesLeft; i++) {
        float knifeX = startX;
        float knifeY = startY - (i * spacing);

        float scaledWidth = GameConstants::getKnifeWidth() * scale;
        float scaledLength = GameConstants::getKnifeLength() * scale;

        SDL_FRect destRect = {
            knifeX - scaledWidth / 2,
            knifeY - scaledLength / 2,
            scaledWidth,
            scaledLength
        };

        // Rotate 45 degrees for indicator style
        SDL_FPoint center = {
            scaledWidth / 2,
            scaledLength / 2
        };

        SDL_RenderTextureRotated(renderer, knifeTexture, nullptr, &destRect,
            45.0f, &center, SDL_FLIP_NONE);
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

    // Title with shadow effect (using dynamic positioning)
    SDL_Color shadowColor = { 0, 0, 0, 128 };
    SDL_Color titleColor = { 255, 255, 255, 255 };

    float centerX = GameConstants::CURRENT_WIDTH / 2;
    float titleY = GameConstants::CURRENT_HEIGHT / 3;
    float shadowOffset = GameConstants::scaleUniform(4);

    renderText("KNIFE", centerX, titleY - GameConstants::getUILineHeight() + shadowOffset,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("KNIFE", centerX, titleY - GameConstants::getUILineHeight(),
        titleColor, true, FontManager::TITLE_FONT);

    renderText("HIT", centerX, titleY + GameConstants::getUILineHeight() + shadowOffset,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("HIT", centerX, titleY + GameConstants::getUILineHeight(),
        titleColor, true, FontManager::TITLE_FONT);

    // Pulsing effect for TAP TO PLAY
    float pulse = (sin(SDL_GetTicks() / 300.0f) + 1.0f) / 2.0f;
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    SDL_Color tapColor = { 255, 255, 255, alpha };

    renderText("TAP TO PLAY", centerX, GameConstants::CURRENT_HEIGHT * 2 / 3,
        tapColor, true, FontManager::UI_FONT);

    present();
}

void Renderer::renderHUD(int level, int score) {
    // Stage indicator (using dynamic positioning)
    std::string stageText = "STAGE " + std::to_string(level);
    renderText(stageText, GameConstants::getUIMargin() + GameConstants::scaleUniform(100), GameConstants::getUITopMargin(), {255, 255, 255, 255}, true, FontManager::UI_FONT);

    // Score number (using dynamic positioning)
    renderText(std::to_string(score), GameConstants::CURRENT_WIDTH - GameConstants::scaleUniform(100), GameConstants::getUITopMargin(), { 255, 255, 255, 255 }, true, FontManager::SCORE_FONT);
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

    float centerX = GameConstants::CURRENT_WIDTH / 2;
    float centerY = GameConstants::CURRENT_HEIGHT / 2;
    float shadowOffset = GameConstants::scaleUniform(4);

    renderText("GAME OVER", centerX, centerY - GameConstants::scaleUniform(150) + shadowOffset,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("GAME OVER", centerX, centerY - GameConstants::scaleUniform(150),
        gameOverColor, true, FontManager::TITLE_FONT);

    std::string scoreText = "SCORE: " + std::to_string(score);
    renderText(scoreText, centerX, centerY - GameConstants::scaleUniform(20),
        { 255, 255, 255, 255 }, true, FontManager::UI_FONT);

    // Pulsing restart button
    float pulse = (sin(SDL_GetTicks() / 300.0f) + 1.0f) / 2.0f;
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    SDL_Color restartColor = { 255, 255, 255, alpha };

    renderText("TAP TO RESTART", centerX, centerY + GameConstants::scaleUniform(90),
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

    float centerX = GameConstants::CURRENT_WIDTH / 2;
    float centerY = GameConstants::CURRENT_HEIGHT / 2;
    float shadowOffset = GameConstants::scaleUniform(4);

    renderText("STAGE", centerX, centerY - GameConstants::scaleUniform(100) + shadowOffset,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("STAGE", centerX, centerY - GameConstants::scaleUniform(100),
        successColor, true, FontManager::TITLE_FONT);

    renderText("COMPLETE!", centerX, centerY + GameConstants::scaleUniform(20) + shadowOffset,
        shadowColor, true, FontManager::TITLE_FONT);

    renderText("COMPLETE!", centerX, centerY + GameConstants::scaleUniform(20),
        successColor, true, FontManager::TITLE_FONT);

    // Tap to continue instruction
    float pulse = (sin(SDL_GetTicks() / 300.0f) + 1.0f) / 2.0f;
    Uint8 alpha = static_cast<Uint8>(128 + 127 * pulse);
    SDL_Color continueColor = { 255, 255, 255, alpha };

    renderText("TAP TO CONTINUE", centerX, centerY + GameConstants::scaleUniform(150),
        continueColor, true, FontManager::UI_FONT);

    present();
}

void Renderer::renderGame(const Target& target, const std::vector<Knife>& knives,
    const Knife& currentKnife, int level, int score, int knivesLeft) {
    clear();
    renderBackground();

    // Render stuck knives FIRST (behind target)
    renderKnives(knives);

    // Render target AFTER knives (on top)
    renderTarget(target);

    // Render current flying knife (always on top)
    if (currentKnife.isKnifeActive() && !currentKnife.isKnifeStuck()) {
        renderKnife(currentKnife, false);
    }

    renderHUD(level, score);
    renderKnifeIndicators(knivesLeft);
    present();
}

void Renderer::renderCollisionPause(const Target& target, const std::vector<Knife>& knives,
    const Knife& currentKnife, int level, int score, int knivesLeft) {
    clear();
    renderBackground();

    // Render stuck knives (behind target)
    renderKnives(knives);

    // Render target
    renderTarget(target);

    // Render HUD
    renderHUD(level, score);
    renderKnifeIndicators(knivesLeft);

    // Add visual feedback for collision
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);  // Red overlay
    SDL_RenderFillRect(renderer, nullptr);

    // Show collision message (using dynamic positioning)
    SDL_Color collisionColor = { 255, 255, 255, 255 };
    float centerX = GameConstants::CURRENT_WIDTH / 2;

    renderText("KNIFE", centerX, GameConstants::CURRENT_HEIGHT - GameConstants::scaleUniform(300),
        collisionColor, true, FontManager::TITLE_FONT);
    renderText("COLLISION!", centerX, GameConstants::CURRENT_HEIGHT - GameConstants::scaleUniform(130),
        collisionColor, true, FontManager::TITLE_FONT);

    present();
}

SDL_Renderer* Renderer::getSDLRenderer() const {
    return renderer;
}