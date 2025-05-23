#define _USE_MATH_DEFINES
#include "../include/Knife.hpp"
#include <cmath>

// Game constants
const float KNIFE_SPEED = 800.0f;
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;

Knife::Knife()
    : x(SCREEN_WIDTH / 2)
    , y(SCREEN_HEIGHT - 150)
    , velY(0)
    , isStuck(false)
    , isActive(true)
    , stuckAngle(0)
    , distanceFromCenter(0) {
}

void Knife::update(float deltaTime) {
    if (isActive && !isStuck) {
        y += velY * deltaTime;
    }
}

void Knife::reset() {
    x = SCREEN_WIDTH / 2;
    y = SCREEN_HEIGHT - 150;
    velY = 0;
    isStuck = false;
    isActive = true;
    stuckAngle = 0;
    distanceFromCenter = 0;
}

void Knife::stick(float targetX, float targetY, float targetRotation) {
    isStuck = true;
    velY = 0;
    
    // Calculate angle relative to target center
    float angle = atan2(y - targetY, x - targetX) * 180.0f / M_PI;
    angle -= targetRotation; // Make relative to target rotation
    if (angle < 0) angle += 360;
    
    stuckAngle = angle;
    distanceFromCenter = sqrt(pow(x - targetX, 2) + pow(y - targetY, 2));
} 