#include "../include/Target.hpp"
#include <random>

Target::Target()
    : x(GameConstants::getTargetX())
    , y(GameConstants::getTargetY())
    , radius(GameConstants::getTargetRadius())
    , rotation(0)
    , rotationSpeed(GameConstants::BASE_ROTATION_SPEED) {
}

void Target::update(float deltaTime) {
    rotation += rotationSpeed * deltaTime;
    if (rotation >= 360) rotation -= 360;
    if (rotation < 0) rotation += 360;
}

void Target::reset(int level) {
    // Update position and radius based on current scaling
    x = GameConstants::getTargetX();
    y = GameConstants::getTargetY();
    radius = GameConstants::getTargetRadius();

    rotation = 0;
    rotationSpeed = GameConstants::BASE_ROTATION_SPEED + (level * GameConstants::ROTATION_SPEED_INCREMENT);

    if (level > GameConstants::LEVEL_FOR_REVERSE_ROTATION) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        rotationSpeed *= (dis(gen) == 0) ? 1 : -1; // Random direction
    }

    stuckKnifeAngles.clear();
    stuckKnifeDistances.clear();

    // Add some pre-stuck knives for higher levels
    if (level > 1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angleDist(0, 360);

        int preStuckKnives = std::min(level - 1, GameConstants::MAX_PRE_STUCK_KNIVES);
        for (int i = 0; i < preStuckKnives; i++) {
            float angle = angleDist(gen);
            stuckKnifeAngles.push_back(angle);
            stuckKnifeDistances.push_back(GameConstants::getKnifeTargetHitThreshold());
        }
    }
}

void Target::addStuckKnife(float angle, float distance) {
    stuckKnifeAngles.push_back(angle);
    stuckKnifeDistances.push_back(distance);
}