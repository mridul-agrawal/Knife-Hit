#define _USE_MATH_DEFINES
#include "../include/Renderer.hpp"
#include <cmath>
#include <iostream>

const float KNIFE_LENGTH = 60.0f;
const float KNIFE_WIDTH = 8.0f;

Renderer::Renderer(SDL_Window* window)
    : window(window)
    , renderer(nullptr) {
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
    return true;
}

void Renderer::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}

void Renderer::drawKnife(float x, float y, float angle) {
    // Knife handle (brown)
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_FRect handleRect = {
        x - KNIFE_WIDTH / 2,
        y,
        KNIFE_WIDTH,
        KNIFE_LENGTH * 0.4f
    };
    SDL_RenderFillRect(renderer, &handleRect);

    // Knife blade (silver)
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_FRect bladeRect = {
        x - KNIFE_WIDTH / 2 + 1,
        y - KNIFE_LENGTH * 0.6f,
        KNIFE_WIDTH - 2,
        KNIFE_LENGTH * 0.6f
    };
    SDL_RenderFillRect(renderer, &bladeRect);

    // Knife tip (darker silver)
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_FRect tipRect = {
        x - 2,
        y - KNIFE_LENGTH * 0.6f - 8,
        4,
        8
    };
    SDL_RenderFillRect(renderer, &tipRect);
}

void Renderer::drawTarget(const Target& target) {
    // Draw target circle
    const int segments = 60;
    const float radius = target.getRadius();
    const float x = target.getX();
    const float y = target.getY();
    const float rotation = target.getRotation();
    
    // Draw circle outline
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2 * M_PI;
        float angle2 = (float)(i + 1) / segments * 2 * M_PI;
        
        SDL_RenderLine(renderer,
            x + cos(angle1 + rotation * M_PI / 180) * radius,
            y + sin(angle1 + rotation * M_PI / 180) * radius,
            x + cos(angle2 + rotation * M_PI / 180) * radius,
            y + sin(angle2 + rotation * M_PI / 180) * radius);
    }
    
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
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255); // Dark blue background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Center text properly (assuming character width = 20)
    int screenCenterX = 200; // SCREEN_WIDTH / 2

    drawText("KNIFE HIT", screenCenterX - 90, 200);    // 9 chars * 20 / 2 = 90
    drawText("Click to Start", screenCenterX - 130, 300); // 13 chars * 20 / 2 = 130

    // Draw a sample target and knife for visual appeal
    SDL_SetRenderDrawColor(renderer, 160, 82, 45, 255); // Brown
    // Draw a simple target circle
    for (int angle = 0; angle < 360; angle += 10) {
        float rad = angle * M_PI / 180.0f;
        float x1 = screenCenterX + cos(rad) * 60;
        float y1 = 250 + sin(rad) * 60;
        float x2 = screenCenterX + cos((angle + 10) * M_PI / 180.0f) * 60;
        float y2 = 250 + sin((angle + 10) * M_PI / 180.0f) * 60;
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }

    // Draw knife at bottom
    drawKnife(screenCenterX, 450);

    SDL_RenderPresent(renderer);
}

void Renderer::renderGame(const Target& target, const Knife& currentKnife, int knivesLeft, int score, int level) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    drawTarget(target);
    
    if (currentKnife.isKnifeActive()) {
        drawKnife(currentKnife.getX(), currentKnife.getY());
    }
    
    drawText("Level: ", 20, 20);
    drawNumber(level, 100, 20);
    
    drawText("Score: ", 20, 50);
    drawNumber(score, 100, 50);
    
    drawText("Knives: ", 20, 80);
    drawNumber(knivesLeft, 100, 80);
    
    SDL_RenderPresent(renderer);
}

void Renderer::renderGameOver(int score) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    drawText("GAME OVER", 300, 400);
    drawText("Score: ", 300, 450);
    drawNumber(score, 400, 450);
    drawText("Click to Restart", 280, 500);
    
    SDL_RenderPresent(renderer);
}

void Renderer::renderLevelComplete(int level, int score) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    drawText("LEVEL COMPLETE", 280, 400);
    drawText("Level: ", 300, 450);
    drawNumber(level, 380, 450);
    drawText("Score: ", 300, 500);
    drawNumber(score, 380, 500);
    drawText("Click to Continue", 280, 550);
    
    SDL_RenderPresent(renderer);
}

void Renderer::drawText(const std::string& text, int x, int y) {
    for (size_t i = 0; i < text.length(); i++) {
        drawCharacter(text[i], x + i * 20, y);
    }
}

void Renderer::drawNumber(int num, int x, int y) {
    std::string numStr = std::to_string(num);
    drawText(numStr, x, y);
}

void Renderer::drawCharacter(char c, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    switch (c) {
    case 'A':
        SDL_RenderLine(renderer, x, y + 20, x + 10, y);      // Left line
        SDL_RenderLine(renderer, x + 10, y, x + 20, y + 20); // Right line
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        break;
    case 'C':
        SDL_RenderLine(renderer, x + 15, y + 5, x + 5, y + 5);   // Top
        SDL_RenderLine(renderer, x + 5, y + 5, x + 5, y + 15);   // Left
        SDL_RenderLine(renderer, x + 5, y + 15, x + 15, y + 15); // Bottom
        break;
    case 'E':
        SDL_RenderLine(renderer, x, y, x, y + 20);      // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x, y + 10, x + 10, y + 10); // Middle
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'F':
        SDL_RenderLine(renderer, x, y, x, y + 20);      // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x, y + 10, x + 10, y + 10); // Middle
        break;
    case 'G':
        SDL_RenderLine(renderer, x + 15, y + 5, x + 5, y + 5);   // Top
        SDL_RenderLine(renderer, x + 5, y + 5, x + 5, y + 15);   // Left
        SDL_RenderLine(renderer, x + 5, y + 15, x + 15, y + 15); // Bottom
        SDL_RenderLine(renderer, x + 15, y + 15, x + 15, y + 10); // Right bottom
        SDL_RenderLine(renderer, x + 10, y + 10, x + 15, y + 10); // Middle
        break;
    case 'H':
        SDL_RenderLine(renderer, x, y, x, y + 20);      // Left
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 10); // Middle
        break;
    case 'I':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 20); // Middle
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'K':
        SDL_RenderLine(renderer, x, y, x, y + 20);          // Left
        SDL_RenderLine(renderer, x, y + 10, x + 15, y);     // Upper diagonal
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 20); // Lower diagonal
        break;
    case 'L':
        SDL_RenderLine(renderer, x, y, x, y + 20);      // Left
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'M':
        SDL_RenderLine(renderer, x, y + 20, x, y);      // Left
        SDL_RenderLine(renderer, x, y, x + 10, y + 10); // Left diagonal
        SDL_RenderLine(renderer, x + 10, y + 10, x + 20, y); // Right diagonal
        SDL_RenderLine(renderer, x + 20, y, x + 20, y + 20); // Right
        break;
    case 'N':
        SDL_RenderLine(renderer, x, y + 20, x, y);      // Left
        SDL_RenderLine(renderer, x, y, x + 15, y + 20); // Diagonal
        SDL_RenderLine(renderer, x + 15, y + 20, x + 15, y); // Right
        break;
    case 'O':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 5, y);  // Left
        break;
    case 'R':
        SDL_RenderLine(renderer, x, y, x, y + 20);      // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 10); // Right top
        SDL_RenderLine(renderer, x + 15, y + 10, x, y + 10); // Middle
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 20); // Diagonal
        break;
    case 'S':
        SDL_RenderLine(renderer, x + 15, y + 5, x + 5, y + 5);   // Top
        SDL_RenderLine(renderer, x + 5, y + 5, x + 5, y + 10);   // Left top
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        SDL_RenderLine(renderer, x + 15, y + 10, x + 15, y + 15); // Right bottom
        SDL_RenderLine(renderer, x + 15, y + 15, x + 5, y + 15); // Bottom
        break;
    case 'T':
        SDL_RenderLine(renderer, x, y, x + 20, y);      // Top
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 20); // Middle
        break;
    case 'V':
        SDL_RenderLine(renderer, x, y, x + 10, y + 20); // Left
        SDL_RenderLine(renderer, x + 20, y, x + 10, y + 20); // Right
        break;
    case 'W':
        SDL_RenderLine(renderer, x, y, x + 5, y + 20);   // Left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 10, y + 10); // Left diagonal
        SDL_RenderLine(renderer, x + 10, y + 10, x + 15, y + 20); // Right diagonal
        SDL_RenderLine(renderer, x + 15, y + 20, x + 20, y); // Right
        break;
    case ' ':
        // Space - do nothing
        break;
    case ':':
        SDL_RenderLine(renderer, x + 5, y + 5, x + 7, y + 7);   // Top dot
        SDL_RenderLine(renderer, x + 5, y + 13, x + 7, y + 15); // Bottom dot
        break;
    default:
        // Draw a rectangle for unimplemented characters
        SDL_RenderLine(renderer, x, y, x + 15, y);      // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x, y + 20); // Bottom
        SDL_RenderLine(renderer, x, y + 20, x, y);      // Left
        break;
    }
}