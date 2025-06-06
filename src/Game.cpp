#include "../include/Game.hpp"
#include "../include/GameConstants.hpp"
#include "../include/CollisionDetector.hpp"  // NEW: Include collision detector
#include <iostream>
#include <cmath>
#include <vector>
#include "../include/FontManager.hpp"
#include <SDL3/SDL_render.h>

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
    , shouldExit(false)
    , screenWidth(1080.0f)
    , screenHeight(1920.0f)
    , dimensionsSet(false) {
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    if (!dimensionsSet) {
        setScreenDimensions(1080.0f, 1920.0f);
    }
    return initializeInternal();
}

bool Game::initialize(float width, float height) {
    setScreenDimensions(width, height);
    return initializeInternal();
}

void Game::setScreenDimensions(float width, float height) {
    screenWidth = width;
    screenHeight = height;
    dimensionsSet = true;

    GameConstants::setScreenDimensions(width, height);

    std::cout << "Screen dimensions set to: " << width << "x" << height << std::endl;
    std::cout << "Scale factors - X: " << GameConstants::SCALE_X
        << ", Y: " << GameConstants::SCALE_Y
        << ", Uniform: " << GameConstants::UNIFORM_SCALE << std::endl;
}

bool Game::initializeInternal() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Knife Hit",
        static_cast<int>(screenWidth),
        static_cast<int>(screenHeight),
        SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!window) {
        std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer = new Renderer(window);
    if (!renderer->initialize()) {
        return false;
    }

    SDL_SetRenderLogicalPresentation(renderer->getSDLRenderer(),
        static_cast<int>(screenWidth),
        static_cast<int>(screenHeight),
        SDL_LOGICAL_PRESENTATION_LETTERBOX);

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
        level = 1;
        score = 0;
        stuckKnives.clear();
        initializeLevel();
        currentState = GameState::MENU;
        break;

    case GameState::LEVEL_COMPLETE:
        level++;
        score += GameConstants::LEVEL_COMPLETE_BONUS;
        stuckKnives.clear();
        initializeLevel();
        currentState = GameState::PLAYING;
        break;
    }
}

void Game::update(float deltaTime) {
    // Handle collision pause state
    if (currentState == GameState::COLLISION_PAUSE) {
        // collisionPauseTimer -= deltaTime;
        target.update(deltaTime);
        updateStuckKnives();

        /*if (collisionPauseTimer <= 0) {
            currentState = GameState::GAME_OVER;
        }*/
        return;
    }

    if (currentState != GameState::PLAYING) return;

    gameTime += deltaTime;
    target.update(deltaTime);
    currentKnife.update(deltaTime);
    updateStuckKnives();

    // NEW COLLISION DETECTION SYSTEM
    if (currentKnife.isKnifeActive() && !currentKnife.isKnifeStuck()) {

        // Check if knife has reached target
        CollisionResult targetCollision = CollisionDetector::checkKnifeTargetCollision(currentKnife, target);

        if (targetCollision.hasCollision) {

            // Check for knife-knife collision
            //if (CollisionDetector::checkKnifeKnifeCollision(currentKnife, stuckKnives, target)) {
            //    // Collision detected - start pause before game over
            //    collisionDetected = true;
            //    collisionPauseTimer = COLLISION_PAUSE_DURATION;
            //    currentState = GameState::COLLISION_PAUSE;

            //    // Still stick the knife for visual feedback
            //    stickKnifeAtCollisionPoint(targetCollision.point);
            //    return;
            //}

            // No collision - stick knife successfully
            stickKnifeAtCollisionPoint(targetCollision.point);
            completeSuccessfulThrow();
        }
    }
}

// NEW: Clean method to stick knife at collision point
void Game::stickKnifeAtCollisionPoint(const CollisionPoint& collisionPoint) {
    currentKnife.stick(target.getX(), target.getY(), target.getRotation());
    target.addStuckKnife(collisionPoint.angle, GameConstants::getTargetRadius());
    stuckKnives.push_back(currentKnife);
}

// NEW: Clean method to handle successful knife throw
void Game::completeSuccessfulThrow() {
    // Reset for next throw
    currentKnife = Knife();
    currentKnife.reset();
    currentKnife.setActive(true);

    score += GameConstants::POINTS_PER_KNIFE;
    canThrow = true;

    /*if (knivesLeft <= 0) {
        currentState = GameState::LEVEL_COMPLETE;
    }*/
}

void Game::updateStuckKnives() {
    for (auto& knife : stuckKnives) {
        knife.updateStuckPosition(target.getX(), target.getY(), target.getRotation());
    }
}

void Game::throwKnife() {
    if (!canThrow || knivesLeft <= 0) return;

    currentKnife.setVelocityY(-GameConstants::getKnifeSpeed());
    currentKnife.setActive(true);
    canThrow = false;
    // knivesLeft--;
}

void Game::initializeLevel() {
    target.reset(level);
    currentKnife = Knife();
    stuckKnives.clear();
    knivesLeft = GameConstants::KNIVES_PER_LEVEL;
    canThrow = true;

    collisionDetected = false;
    collisionPauseTimer = 0.0f;

    currentKnife.reset();
    currentKnife.setActive(true);
}

void Game::run() {
#ifdef __EMSCRIPTEN__
    return;
#else
    while (!shouldExit) {
        if (!loop()) break;
        SDL_Delay(16);
    }
#endif
}

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