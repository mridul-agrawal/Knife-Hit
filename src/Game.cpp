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
    , collisionDetected(false)
    , collisionPauseTimer(0.0f) 
    , shouldExit(false) {     // NEW
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
            shouldExit = true;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (e.key.key == SDLK_ESCAPE) {
                shouldExit = true;
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

bool Game::checkKnifeCollision() {
    if (stuckKnives.empty()) {
        std::cout << "No stuck knives to check collision against" << std::endl;
        return false;
    }

    std::cout << "\n=== COLLISION CHECK START ===" << std::endl;
    std::cout << "Number of stuck knives: " << stuckKnives.size() << std::endl;
    std::cout << "Collision threshold: " << GameConstants::SPATIAL_COLLISION_THRESHOLD << "px" << std::endl;

    // Get current knife position and target info
    float incomingX = currentKnife.getX();
    float incomingY = currentKnife.getY();
    float targetX = target.getX();
    float targetY = target.getY();
    float targetRotation = target.getRotation();

    std::cout << "Incoming knife pos: (" << incomingX << ", " << incomingY << ")" << std::endl;
    std::cout << "Target pos: (" << targetX << ", " << targetY << "), rotation: " << targetRotation << "°" << std::endl;

    // Calculate incoming knife's angle relative to target
    float incomingAngle = atan2(incomingY - targetY, incomingX - targetX) * 180.0f / M_PI;
    float originalAngle = incomingAngle;

    // Normalize the angle relative to target's current rotation
    incomingAngle -= targetRotation;
    if (incomingAngle < 0) incomingAngle += 360;
    if (incomingAngle >= 360) incomingAngle -= 360;

    std::cout << "Incoming knife angle: " << originalAngle << "° (absolute) -> " << incomingAngle << "° (relative)" << std::endl;

    // Check against each stuck knife
    for (size_t i = 0; i < stuckKnives.size(); i++) {
        const auto& stuckKnife = stuckKnives[i];
        float stuckAngle = stuckKnife.getStuckAngle();

        std::cout << "\nChecking against stuck knife " << i << ":" << std::endl;
        std::cout << "  Stuck angle: " << stuckAngle << "°" << std::endl;

        // Calculate angular difference
        float angleDiff = std::abs(incomingAngle - stuckAngle);
        if (angleDiff > 180) {
            angleDiff = 360 - angleDiff;
        }

        std::cout << "  Angular difference: " << angleDiff << "°" << std::endl;

        // Convert angular difference to physical distance at handle position
        float handleRadius = GameConstants::TARGET_RADIUS + GameConstants::KNIFE_IMAGE_HANDLE_OFFSET;
        float physicalSeparation = (angleDiff * M_PI / 180.0f) * handleRadius;

        std::cout << "  Handle radius: " << handleRadius << "px" << std::endl;
        std::cout << "  Physical separation: " << physicalSeparation << "px" << std::endl;
        std::cout << "  Collision threshold: " << GameConstants::SPATIAL_COLLISION_THRESHOLD << "px" << std::endl;

        // Check if handles would overlap/collide
        if (physicalSeparation < GameConstants::SPATIAL_COLLISION_THRESHOLD) {
            std::cout << "  🔴 COLLISION! " << physicalSeparation << " < " << GameConstants::SPATIAL_COLLISION_THRESHOLD << std::endl;
            std::cout << "=== COLLISION CHECK END (COLLISION FOUND) ===" << std::endl;
            return true;
        }
        else {
            std::cout << "  ✅ No collision with this knife" << std::endl;
        }
    }

    std::cout << "✅ No collisions detected with any stuck knife" << std::endl;
    std::cout << "=== COLLISION CHECK END (NO COLLISION) ===" << std::endl;
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
#ifdef __EMSCRIPTEN__
    // Emscripten will handle the main loop
    return;
#else
    while (!shouldExit) {
        if (!loop()) break;
        SDL_Delay(16);  // ~60 FPS
    }
#endif
}

// NEW METHOD - Add this to Game.cpp
bool Game::loop() {
    handleEvents();

    if (shouldExit) {
        return false;
    }

    Uint64 currentTime = SDL_GetTicksNS();
    float deltaTime = (currentTime - lastTime) / 1e9f;
    lastTime = currentTime;

    update(deltaTime);

    switch (currentState) {
    case GameState::MENU:
        renderer->renderMenu();
        break;

    case GameState::PLAYING:
        renderer->renderGame(target, stuckKnives, currentKnife, level, score, knivesLeft);
        break;

    case GameState::COLLISION_PAUSE:
        renderer->renderCollisionPause(target, stuckKnives, currentKnife, level, score, knivesLeft);
        break;

    case GameState::GAME_OVER:
        renderer->renderGameOver(score);
        break;

    case GameState::LEVEL_COMPLETE:
        renderer->renderLevelComplete();
        break;
    }

    return true;
}