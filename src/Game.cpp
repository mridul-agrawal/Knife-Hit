#include "../include/Game.hpp"
#include "../include/GameConstants.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include "../include/FontManager.hpp"

Game::Game()
    : window(nullptr)
    , renderer(nullptr)
    , currentState(GameState::MENU)
    , level(1)
    , knivesLeft(GameConstants::KNIVES_PER_LEVEL)
    , score(0)
    , canThrow(true)
    , lastTime(0)
    , gameTime(0)
    , collisionDetected(false)        // NEW
    , collisionPauseTimer(0.0f) {     // NEW
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Knife Hit",
        GameConstants::SCREEN_WIDTH,
        GameConstants::SCREEN_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!window) {
        std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set window to centered position
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer = new Renderer(window);
    if (!renderer->initialize()) {
        return false;
    }

    lastTime = SDL_GetTicksNS();
    initializeLevel();
    return true;
}

void Game::cleanup() {
    if (renderer) {
        delete renderer;
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_QUIT:
            // FIXED: Properly exit the game
            cleanup();
            exit(0);
            break;

        case SDL_EVENT_KEY_DOWN:
            if (e.key.key == SDLK_ESCAPE) {
                cleanup();
                exit(0);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                handleInput();
            }
            break;

        case SDL_EVENT_FINGER_DOWN:
            handleInput();
            break;
        }
    }
}

void Game::handleInput() {
    switch (currentState) {
    case GameState::MENU:
        currentState = GameState::PLAYING;
        break;

    case GameState::PLAYING:
        if (canThrow && knivesLeft > 0 && !currentKnife.isKnifeStuck()) {
            throwKnife();
        }
        break;

    case GameState::GAME_OVER:
        // Reset game state
        level = 1;
        score = 0;
        stuckKnives.clear();  // Clear stuck knives
        initializeLevel();
        currentState = GameState::MENU;
        break;

    case GameState::LEVEL_COMPLETE:
        level++;
        score += GameConstants::LEVEL_COMPLETE_BONUS;  // Bonus points
        stuckKnives.clear();  // FIXED: Clear stuck knives before initializing new level
        initializeLevel();
        currentState = GameState::PLAYING;
        break;
    }
}

void Game::update(float deltaTime) {

    // Handle collision pause state
    if (currentState == GameState::COLLISION_PAUSE) {
        collisionPauseTimer -= deltaTime;
        target.update(deltaTime);  // Keep target rotating during pause
        updateStuckKnives();       // Keep knives rotating with target

        if (collisionPauseTimer <= 0) {
            currentState = GameState::GAME_OVER;
        }
        return;  // Don't process other game logic during pause
    }

    if (currentState != GameState::PLAYING) return;

    gameTime += deltaTime;
    target.update(deltaTime);
    currentKnife.update(deltaTime);

    // Update stuck knives positions as target rotates
    updateStuckKnives();

    if (currentKnife.isKnifeActive() && !currentKnife.isKnifeStuck()) {
        // FIXED: Check if knife tip reaches the edge of target
        float knifeBottom = currentKnife.getY() + GameConstants::KNIFE_LENGTH / 2;
        float knifeTop = currentKnife.getY() - GameConstants::KNIFE_IMAGE_TIP_OFFSET;
        float targetEdge = target.getY() + target.getRadius();

        if (knifeTop >= targetEdge - GameConstants::KNIFE_TARGET_HIT_THRESHOLD &&
            currentKnife.getY() <= target.getY() + target.getRadius() + 30) {

            // Check collision with stuck knives ONLY when knife reaches target
            if (checkKnifeCollision()) {
                // Don't immediately go to game over - pause for visual feedback
                collisionDetected = true;
                collisionPauseTimer = COLLISION_PAUSE_DURATION;
                currentState = GameState::COLLISION_PAUSE;

                // Still stick the knife at collision point for visual feedback
                float angle = atan2(currentKnife.getY() - target.getY(),
                    currentKnife.getX() - target.getX()) * 180.0f / M_PI;
                angle -= target.getRotation();
                if (angle < 0) angle += 360;

                currentKnife.stick(target.getX(), target.getY(), target.getRotation());
                target.addStuckKnife(angle, GameConstants::TARGET_RADIUS);
                stuckKnives.push_back(currentKnife);

                return;
            }

            // Calculate angle and stick the knife AT THE EDGE
            float angle = atan2(currentKnife.getY() - target.getY(),
                currentKnife.getX() - target.getX()) * 180.0f / M_PI;
            angle -= target.getRotation();
            if (angle < 0) angle += 360;

            // Position knife at the edge of the target
            currentKnife.stick(target.getX(), target.getY(), target.getRotation());
            target.addStuckKnife(angle, GameConstants::TARGET_RADIUS);

            // Add to stuck knives collection
            stuckKnives.push_back(currentKnife);

            // Reset for next throw
            currentKnife = Knife();
            currentKnife.reset();
            currentKnife.setActive(true);

            score += GameConstants::POINTS_PER_KNIFE;
            canThrow = true;

            if (knivesLeft <= 0) {
                currentState = GameState::LEVEL_COMPLETE;
            }
        }
    }
}

// REPLACE the entire checkKnifeCollision() method:
bool Game::checkKnifeCollision() {
    if (stuckKnives.empty()) {
        return false;
    }

    // Get incoming knife position when it hits target edge
    float incomingX = currentKnife.getX();
    float incomingY = currentKnife.getY();
    float targetX = target.getX();
    float targetY = target.getY();

    // Calculate where the incoming knife's handle will be when stuck
    float incomingAngle = atan2(incomingY - targetY, incomingX - targetX);
    float handleDistance = GameConstants::TARGET_RADIUS + GameConstants::KNIFE_IMAGE_HANDLE_OFFSET;
    float predictedHandleX = targetX + handleDistance * cos(incomingAngle);
    float predictedHandleY = targetY + handleDistance * sin(incomingAngle);

    // Check collision with each stuck knife
    for (const auto& stuckKnife : stuckKnives) {
        // Get the stuck knife's current handle position
        float stuckHandleX = stuckKnife.getHandleX();
        float stuckHandleY = stuckKnife.getHandleY();

        // Calculate physical distance between handles
        float handleDx = predictedHandleX - stuckHandleX;
        float handleDy = predictedHandleY - stuckHandleY;
        float handleDistance = sqrt(handleDx * handleDx + handleDy * handleDy);

        // Check handle collision (most visible to player)
        if (handleDistance < GameConstants::SPATIAL_COLLISION_THRESHOLD) {
            std::cout << "HANDLE COLLISION! Distance: " << handleDistance
                << "px (threshold: " << GameConstants::SPATIAL_COLLISION_THRESHOLD << "px)" << std::endl;
            return true;
        }

        // Also check blade collision for very close cases
        float stuckBladeX = stuckKnife.getBladeX();
        float stuckBladeY = stuckKnife.getBladeY();

        // Predict incoming blade position
        float bladeDistance = GameConstants::TARGET_RADIUS - GameConstants::KNIFE_IMAGE_TIP_OFFSET;
        float predictedBladeX = targetX + bladeDistance * cos(incomingAngle);
        float predictedBladeY = targetY + bladeDistance * sin(incomingAngle);

        float bladeDx = predictedBladeX - stuckBladeX;
        float bladeDy = predictedBladeY - stuckBladeY;
        float bladeCollisionDistance = sqrt(bladeDx * bladeDx + bladeDy * bladeDy);

        // Blade collision threshold (smaller since blades are thinner)
        if (bladeCollisionDistance < GameConstants::BLADE_WIDTH + 2.0f) {
            std::cout << "BLADE COLLISION! Distance: " << bladeCollisionDistance
                << "px (threshold: " << GameConstants::BLADE_WIDTH + 2.0f << "px)" << std::endl;
            return true;
        }
    }

    return false;
}

void Game::updateStuckKnives() {
    // Update all stuck knives to rotate with the target
    for (auto& knife : stuckKnives) {
        knife.updateStuckPosition(target.getX(), target.getY(), target.getRotation());
    }
}

void Game::throwKnife() {
    if (!canThrow || knivesLeft <= 0) return;  // Safety check

    currentKnife.setVelocityY(-GameConstants::KNIFE_SPEED);
    currentKnife.setActive(true);
    canThrow = false;
    knivesLeft--;
}

void Game::initializeLevel() {
    target.reset(level);
    currentKnife = Knife(); // Reset knife to starting position
    stuckKnives.clear();  // Clear stuck knives for new level
    knivesLeft = GameConstants::KNIVES_PER_LEVEL;
    canThrow = true;

    // NEW: Reset collision state
    collisionDetected = false;
    collisionPauseTimer = 0.0f;

    // FIXED: Ensure knife is properly positioned and visible
    currentKnife.reset(); // Make sure knife is at starting position
    currentKnife.setActive(true); // Make sure knife is active and visible
}

void Game::run() {
    while (true) {  // FIXED: Use infinite loop with proper exit handling
        handleEvents();

        Uint64 currentTime = SDL_GetTicksNS();
        float deltaTime = (currentTime - lastTime) / 1e9f;
        lastTime = currentTime;

        update(deltaTime);

        switch (currentState) {
        case GameState::MENU:
            renderer->renderMenu();
            break;

        case GameState::PLAYING:
            // Pass actual stuck knives instead of empty vector
            renderer->renderGame(target, stuckKnives, currentKnife, level, score, knivesLeft);
            break;

        case GameState::COLLISION_PAUSE:  // NEW: Show collision state
            renderer->renderCollisionPause(target, stuckKnives, currentKnife, level, score, knivesLeft);
            break;

        case GameState::GAME_OVER:
            renderer->renderGameOver(score);
            break;  // Don't exit, wait for input

        case GameState::LEVEL_COMPLETE:
            renderer->renderLevelComplete();
            // FIXED: Don't auto-advance, wait for user input only
            break;
        }

        // Small delay to prevent excessive CPU usage
        SDL_Delay(16);  // ~60 FPS
    }
}