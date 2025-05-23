#include "../include/Target.hpp"
#include <random>

// Game constants
const int SCREEN_WIDTH = 400;
const float TARGET_RADIUS = 80.0f;

Target::Target()
    : x(SCREEN_WIDTH / 2)
    , y(300)
    , radius(TARGET_RADIUS)
    , rotation(0)
    , rotationSpeed(60.0f) {
}

void Target::update(float deltaTime) {
    rotation += rotationSpeed * deltaTime;
    if (rotation >= 360) rotation -= 360;
    if (rotation < 0) rotation += 360;
}

void Target::reset(int level) {
    rotation = 0;
    rotationSpeed = 30.0f + (level * 15.0f);
    
    if (level > 3) {
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
        
        int preStuckKnives = std::min(level - 1, 4);
        for (int i = 0; i < preStuckKnives; i++) {
            float angle = angleDist(gen);
            stuckKnifeAngles.push_back(angle);
            stuckKnifeDistances.push_back(radius + 30);
        }
    }
}

void Target::addStuckKnife(float angle, float distance) {
    stuckKnifeAngles.push_back(angle);
    stuckKnifeDistances.push_back(distance);
} 