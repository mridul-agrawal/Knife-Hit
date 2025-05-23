#include "../include/Renderer.hpp"
#include <iostream>

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

void Renderer::setColor(const GameConstants::Colors::Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Renderer::drawKnife(float x, float y, float angle) {
    // Knife handle (brown)
    setColor(GameConstants::Colors::BROWN);
    SDL_FRect handleRect = {
        x - GameConstants::KNIFE_WIDTH / 2,
        y,
        GameConstants::KNIFE_WIDTH,
        GameConstants::KNIFE_LENGTH * 0.4f
    };
    SDL_RenderFillRect(renderer, &handleRect);

    // Knife blade (silver)
    setColor(GameConstants::Colors::SILVER);
    SDL_FRect bladeRect = {
        x - GameConstants::KNIFE_WIDTH / 2 + 1,
        y - GameConstants::KNIFE_LENGTH * 0.6f,
        GameConstants::KNIFE_WIDTH - 2,
        GameConstants::KNIFE_LENGTH * 0.6f
    };
    SDL_RenderFillRect(renderer, &bladeRect);

    // Knife tip (darker silver)
    setColor(GameConstants::Colors::DARK_SILVER);
    SDL_FRect tipRect = {
        x - 2,
        y - GameConstants::KNIFE_LENGTH * 0.6f - 8,
        4,
        8
    };
    SDL_RenderFillRect(renderer, &tipRect);
}

void Renderer::drawTarget(const Target& target) {
    const int segments = 60;
    const float radius = target.getRadius();
    const float x = target.getX();
    const float y = target.getY();
    const float rotation = target.getRotation();

    // Draw target base (wood color)
    setColor(GameConstants::Colors::WOOD);
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2 * M_PI;
        float angle2 = (float)(i + 1) / segments * 2 * M_PI;

        SDL_RenderLine(renderer,
            x + cos(angle1 + rotation * M_PI / 180) * radius,
            y + sin(angle1 + rotation * M_PI / 180) * radius,
            x + cos(angle2 + rotation * M_PI / 180) * radius,
            y + sin(angle2 + rotation * M_PI / 180) * radius);
    }

    // Draw target rings
    setColor(GameConstants::Colors::DARK_WOOD);
    for (int ring = 1; ring <= 3; ring++) {
        float ringRadius = radius * ring / 3;
        for (int i = 0; i < 360; i += 15) {
            float angle = i * M_PI / 180.0f + rotation * M_PI / 180;
            float dotX = x + cos(angle) * ringRadius;
            float dotY = y + sin(angle) * ringRadius;
            SDL_FRect dot = { dotX - 1, dotY - 1, 2, 2 };
            SDL_RenderFillRect(renderer, &dot);
        }
    }

    // Draw center dot
    setColor(GameConstants::Colors::RED);
    SDL_FRect center = { x - 3, y - 3, 6, 6 };
    SDL_RenderFillRect(renderer, &center);

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
    // Dark blue background
    setColor(GameConstants::Colors::DARK_BLUE);
    SDL_RenderClear(renderer);

    setColor(GameConstants::Colors::WHITE);

    // Center the text properly
    int centerX = GameConstants::SCREEN_WIDTH / 2;

    // "KNIFE HIT" - 9 characters * 20 pixels = 180 pixels wide
    drawText("KNIFE HIT", centerX - 90, 200);

    // "CLICK TO START" - 14 characters * 20 pixels = 280 pixels wide  
    drawText("CLICK TO START", centerX - 140, 350);

    // Draw sample target
    setColor(GameConstants::Colors::WOOD);
    Target sampleTarget;
    drawTarget(sampleTarget);

    // Draw sample knife
    drawKnife(centerX, GameConstants::KNIFE_START_Y);

    SDL_RenderPresent(renderer);
}

void Renderer::renderGame(const Target& target, const Knife& currentKnife, int knivesLeft, int score, int level) {
    // Clear with dark background
    setColor(GameConstants::Colors::BLACK);
    SDL_RenderClear(renderer);

    // Draw target
    drawTarget(target);

    // Draw current knife
    if (currentKnife.isKnifeActive()) {
        drawKnife(currentKnife.getX(), currentKnife.getY());
    }

    // Draw UI
    setColor(GameConstants::Colors::WHITE);
    drawText("LEVEL:", GameConstants::UI_MARGIN, GameConstants::UI_MARGIN);
    drawNumber(level, GameConstants::UI_MARGIN + 120, GameConstants::UI_MARGIN);

    drawText("SCORE:", GameConstants::UI_MARGIN, GameConstants::UI_MARGIN + GameConstants::UI_LINE_HEIGHT);
    drawNumber(score, GameConstants::UI_MARGIN + 120, GameConstants::UI_MARGIN + GameConstants::UI_LINE_HEIGHT);

    drawText("KNIVES:", GameConstants::UI_MARGIN, GameConstants::UI_MARGIN + GameConstants::UI_LINE_HEIGHT * 2);
    drawNumber(knivesLeft, GameConstants::UI_MARGIN + 140, GameConstants::UI_MARGIN + GameConstants::UI_LINE_HEIGHT * 2);

    // Draw knife indicators at bottom
    for (int i = 0; i < knivesLeft; i++) {
        float x = GameConstants::SCREEN_WIDTH / 2 - (knivesLeft * 15) + i * 30;
        drawKnife(x, GameConstants::SCREEN_HEIGHT - 80);
    }

    SDL_RenderPresent(renderer);
}

void Renderer::renderGameOver(int score) {
    setColor(GameConstants::Colors::BLACK);
    SDL_RenderClear(renderer);

    int centerX = GameConstants::SCREEN_WIDTH / 2;

    setColor(GameConstants::Colors::RED);
    drawText("GAME OVER", centerX - 90, 250);

    setColor(GameConstants::Colors::WHITE);
    drawText("SCORE:", centerX - 60, 300);
    drawNumber(score, centerX + 20, 300);

    drawText("CLICK TO RESTART", centerX - 160, 400);

    SDL_RenderPresent(renderer);
}

void Renderer::renderLevelComplete(int level, int score) {
    setColor(GameConstants::Colors::BLACK);
    SDL_RenderClear(renderer);

    int centerX = GameConstants::SCREEN_WIDTH / 2;

    setColor(GameConstants::Colors::GREEN);
    drawText("LEVEL COMPLETE", centerX - 130, 250);

    setColor(GameConstants::Colors::WHITE);
    drawText("LEVEL:", centerX - 60, 300);
    drawNumber(level, centerX + 20, 300);

    drawText("SCORE:", centerX - 60, 350);
    drawNumber(score, centerX + 20, 350);

    drawText("CLICK TO CONTINUE", centerX - 160, 450);

    SDL_RenderPresent(renderer);
}

void Renderer::drawText(const std::string& text, int x, int y) {
    for (size_t i = 0; i < text.length(); i++) {
        drawCharacter(text[i], x + i * GameConstants::CHARACTER_WIDTH, y);
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
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Cross bar
        break;
    case 'B':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left line
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 10); // Right top
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 10); // Middle
        SDL_RenderLine(renderer, x + 15, y + 10, x + 15, y + 20); // Right bottom
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'C':
        SDL_RenderLine(renderer, x + 15, y, x + 5, y);       // Top
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 20);   // Left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'D':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y, x + 12, y);           // Top
        SDL_RenderLine(renderer, x + 12, y, x + 15, y + 5);  // Top curve
        SDL_RenderLine(renderer, x + 15, y + 5, x + 15, y + 15); // Right
        SDL_RenderLine(renderer, x + 15, y + 15, x + 12, y + 20); // Bottom curve
        SDL_RenderLine(renderer, x + 12, y + 20, x, y + 20); // Bottom
        break;
    case 'E':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x, y + 10, x + 12, y + 10); // Middle
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'F':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x, y + 10, x + 12, y + 10); // Middle
        break;
    case 'G':
        SDL_RenderLine(renderer, x + 15, y, x + 5, y);       // Top
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 20);   // Left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 15, y + 20, x + 15, y + 12); // Right bottom
        SDL_RenderLine(renderer, x + 12, y + 12, x + 15, y + 12); // Middle bar
        break;
    case 'H':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 10); // Middle
        break;
    case 'I':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 20); // Middle
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'J':
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 15); // Right
        SDL_RenderLine(renderer, x + 10, y + 15, x + 5, y + 20); // Bottom curve
        SDL_RenderLine(renderer, x + 5, y + 20, x, y + 15);  // Left curve
        break;
    case 'K':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y + 10, x + 15, y);      // Upper diagonal
        SDL_RenderLine(renderer, x, y + 10, x + 15, y + 20); // Lower diagonal
        break;
    case 'L':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case 'M':
        SDL_RenderLine(renderer, x, y + 20, x, y);           // Left
        SDL_RenderLine(renderer, x, y, x + 10, y + 10);      // Left diagonal
        SDL_RenderLine(renderer, x + 10, y + 10, x + 20, y); // Right diagonal  
        SDL_RenderLine(renderer, x + 20, y, x + 20, y + 20); // Right
        break;
    case 'N':
        SDL_RenderLine(renderer, x, y + 20, x, y);           // Left
        SDL_RenderLine(renderer, x, y, x + 15, y + 20);      // Diagonal
        SDL_RenderLine(renderer, x + 15, y + 20, x + 15, y); // Right
        break;
    case 'O':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 5, y);   // Left
        break;
    case 'P':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 10); // Right top
        SDL_RenderLine(renderer, x + 15, y + 10, x, y + 10); // Middle
        break;
    case 'Q':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 15); // Right
        SDL_RenderLine(renderer, x + 15, y + 15, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 5, y);   // Left
        SDL_RenderLine(renderer, x + 12, y + 15, x + 18, y + 20); // Tail
        break;
    case 'R':
        SDL_RenderLine(renderer, x, y, x, y + 20);           // Left
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 10); // Right top
        SDL_RenderLine(renderer, x + 15, y + 10, x, y + 10); // Middle
        SDL_RenderLine(renderer, x + 8, y + 10, x + 15, y + 20); // Diagonal
        break;
    case 'S':
        SDL_RenderLine(renderer, x + 15, y, x + 5, y);       // Top
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 10);   // Left top
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        SDL_RenderLine(renderer, x + 15, y + 10, x + 15, y + 20); // Right bottom
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        break;
    case 'T':
        SDL_RenderLine(renderer, x, y, x + 20, y);           // Top
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 20); // Middle
        break;
    case 'U':
        SDL_RenderLine(renderer, x, y, x, y + 15);           // Left
        SDL_RenderLine(renderer, x, y + 15, x + 5, y + 20);  // Left curve
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 15, y + 20, x + 20, y + 15); // Right curve
        SDL_RenderLine(renderer, x + 20, y + 15, x + 20, y); // Right
        break;
    case 'V':
        SDL_RenderLine(renderer, x, y, x + 10, y + 20);      // Left
        SDL_RenderLine(renderer, x + 20, y, x + 10, y + 20); // Right
        break;
    case 'W':
        SDL_RenderLine(renderer, x, y, x + 5, y + 20);       // Left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 10, y + 10); // Left inner
        SDL_RenderLine(renderer, x + 10, y + 10, x + 15, y + 20); // Right inner
        SDL_RenderLine(renderer, x + 15, y + 20, x + 20, y); // Right
        break;
    case 'X':
        SDL_RenderLine(renderer, x, y, x + 15, y + 20);      // Left diagonal
        SDL_RenderLine(renderer, x + 15, y, x, y + 20);      // Right diagonal
        break;
    case 'Y':
        SDL_RenderLine(renderer, x, y, x + 10, y + 10);      // Left
        SDL_RenderLine(renderer, x + 20, y, x + 10, y + 10); // Right
        SDL_RenderLine(renderer, x + 10, y + 10, x + 10, y + 20); // Bottom
        break;
    case 'Z':
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 15, y, x, y + 20);      // Diagonal
        SDL_RenderLine(renderer, x, y + 20, x + 15, y + 20); // Bottom
        break;
    case '0':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 5, y);   // Left
        SDL_RenderLine(renderer, x + 5, y, x + 15, y + 20);  // Diagonal
        break;
    case '1':
        SDL_RenderLine(renderer, x + 10, y, x + 10, y + 20); // Middle
        SDL_RenderLine(renderer, x + 5, y + 5, x + 10, y);   // Top left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        break;
    case '2':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 10); // Right top
        SDL_RenderLine(renderer, x + 15, y + 10, x + 5, y + 10); // Middle
        SDL_RenderLine(renderer, x + 5, y + 10, x + 5, y + 20); // Left bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        break;
    case '3':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 10, y + 10, x + 15, y + 10); // Middle
        break;
    case '4':
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 10);   // Left top
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        break;
    case '5':
        SDL_RenderLine(renderer, x + 15, y, x + 5, y);       // Top
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 10);   // Left top
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        SDL_RenderLine(renderer, x + 15, y + 10, x + 15, y + 20); // Right bottom
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        break;
    case '6':
        SDL_RenderLine(renderer, x + 15, y, x + 5, y);       // Top
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 20);   // Left
        SDL_RenderLine(renderer, x + 5, y + 20, x + 15, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 15, y + 20, x + 15, y + 10); // Right bottom
        SDL_RenderLine(renderer, x + 15, y + 10, x + 5, y + 10); // Middle
        break;
    case '7':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 10, y + 20); // Diagonal
        break;
    case '8':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y + 20, x + 5, y);   // Left
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        break;
    case '9':
        SDL_RenderLine(renderer, x + 5, y, x + 15, y);       // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x + 5, y + 20); // Bottom
        SDL_RenderLine(renderer, x + 5, y, x + 5, y + 10);   // Left top
        SDL_RenderLine(renderer, x + 5, y + 10, x + 15, y + 10); // Middle
        break;
    case ' ':
        // Space - do nothing
        break;
    case ':':
        // Two dots - using curly braces to create scope for variables
    {
        SDL_FRect dot1 = { (float)x + 8, (float)y + 6, 2, 2 };
        SDL_FRect dot2 = { (float)x + 8, (float)y + 14, 2, 2 };
        SDL_RenderFillRect(renderer, &dot1);
        SDL_RenderFillRect(renderer, &dot2);
    }
    break;
        break;
    default:
        // Unknown character - draw a box
        SDL_RenderLine(renderer, x, y, x + 15, y);           // Top
        SDL_RenderLine(renderer, x + 15, y, x + 15, y + 20); // Right
        SDL_RenderLine(renderer, x + 15, y + 20, x, y + 20); // Bottom
        SDL_RenderLine(renderer, x, y + 20, x, y);           // Left
        break;
    }
}