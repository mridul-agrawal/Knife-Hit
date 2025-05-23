#include "../include/Game.hpp"
#include "../include/GameConstants.hpp"
#include <iostream>
#include <cmath>

Game::Game()
    : window(nullptr)
    , renderer(nullptr)
    , currentState(GameState::MENU)
    , level(1)
    , knivesLeft(GameConstants::KNIVES_PER_LEVEL)
    , score(0)
    , canThrow(true)
    , lastTime(0)
    , gameTime(0) {
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Knife Throw - SDL3 Game",
        GameConstants::SCREEN_WIDTH, GameConstants::SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

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
            currentState = GameState::GAME_OVER;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (e.key.key == SDLK_ESCAPE) {
                currentState = GameState::GAME_OVER;
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
        if (canThrow && knivesLeft > 0) {
            throwKnife();
        }
        break;

    case GameState::GAME_OVER:
        level = 1;
        score = 0;
        initializeLevel();
        currentState = GameState::MENU;
        break;

    case GameState::LEVEL_COMPLETE:
        level++;
        initializeLevel();
        currentState = GameState::PLAYING;
        break;
    }
}

void Game::update(float deltaTime) {
    if (currentState != GameState::PLAYING) return;

    gameTime += deltaTime;
    target.update(deltaTime);
    currentKnife.update(deltaTime);

    if (currentKnife.isKnifeActive() && !currentKnife.isKnifeStuck()) {
        float distToTarget = sqrt(pow(currentKnife.getX() - target.getX(), 2) +
            pow(currentKnife.getY() - target.getY(), 2));

        if (distToTarget <= GameConstants::TARGET_HIT_DISTANCE &&
            currentKnife.getY() <= target.getY() + target.getRadius()) {

            if (checkKnifeCollision()) {
                currentState = GameState::GAME_OVER;
                return;
            }

            float angle = atan2(currentKnife.getY() - target.getY(),
                currentKnife.getX() - target.getX()) * 180.0f / M_PI;
            angle -= target.getRotation();
            if (angle < 0) angle += 360;

            currentKnife.stick(target.getX(), target.getY(), target.getRotation());
            target.addStuckKnife(angle, distToTarget);

            currentKnife = Knife();
            score += GameConstants::POINTS_PER_KNIFE;
            canThrow = true;

            if (knivesLeft <= 0) {
                currentState = GameState::LEVEL_COMPLETE;
            }
        }
    }
}

bool Game::checkKnifeCollision() {
    float knifeAngle = atan2(currentKnife.getY() - target.getY(),
        currentKnife.getX() - target.getX()) * 180.0f / M_PI;
    knifeAngle -= target.getRotation();
    if (knifeAngle < 0) knifeAngle += 360;

    const auto& stuckAngles = target.getStuckKnifeAngles();
    for (float stuckAngle : stuckAngles) {
        float angleDiff = std::abs(knifeAngle - stuckAngle);
        if (angleDiff <= GameConstants::COLLISION_THRESHOLD / 2 ||
            angleDiff >= 360 - GameConstants::COLLISION_THRESHOLD / 2) {
            return true;
        }
    }
    return false;
}

void Game::throwKnife() {
    currentKnife.setVelocityY(-GameConstants::KNIFE_SPEED);
    currentKnife.setActive(true);
    canThrow = false;
    knivesLeft--;
}

void Game::initializeLevel() {
    target.reset(level);
    currentKnife = Knife();
    knivesLeft = GameConstants::KNIVES_PER_LEVEL;
    canThrow = true;
}

void Game::run() {
    while (currentState != GameState::GAME_OVER) {
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
            renderer->renderGame(target, currentKnife, knivesLeft, score, level);
            break;

        case GameState::GAME_OVER:
            renderer->renderGameOver(score);
            break;

        case GameState::LEVEL_COMPLETE:
            renderer->renderLevelComplete(level, score);
            break;
        }
    }
} 