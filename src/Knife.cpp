#include "../include/Knife.hpp"
#include <cmath>

Knife::Knife()
    : x(GameConstants::TARGET_X)
    , y(GameConstants::KNIFE_START_Y)
    , velY(0)
    , isStuck(false)
    , isActive(true)
    , stuckAngle(0)
    , distanceFromCenter(0)
    , rotation(0) {  // NEW: Initialize rotation
}

void Knife::update(float deltaTime) {
    if (isActive && !isStuck) {
        y += velY * deltaTime;
    }
}

void Knife::reset() {
    x = GameConstants::TARGET_X;
    y = GameConstants::KNIFE_START_Y;
    velY = 0;
    isStuck = false;
    isActive = true;
    stuckAngle = 0;
    distanceFromCenter = 0;
    rotation = 0;  // NEW: Reset rotation
}

void Knife::stick(float targetX, float targetY, float targetRotation) {
    isStuck = true;
    velY = 0;

    // Calculate angle relative to target center
    float angle = atan2(y - targetY, x - targetX) * 180.0f / M_PI;
    angle -= targetRotation; // Make relative to target rotation
    if (angle < 0) angle += 360;

    stuckAngle = angle;
    // FIXED: Set distance to target radius so knife sticks at edge
    distanceFromCenter = GameConstants::TARGET_RADIUS;

    // FIXED: Position knife exactly at the edge of target
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;
    x = targetX + distanceFromCenter * cos(currentAngle);
    y = targetY + distanceFromCenter * sin(currentAngle);

    // Keep knife upright (no rotation for visual consistency)
    rotation = 0.0f;
}

// NEW: Update position of stuck knife as target rotates
void Knife::updateStuckPosition(float targetX, float targetY, float targetRotation) {
    if (!isStuck) return;

    // Calculate current angle based on target rotation
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;

    // Update position based on distance and angle
    x = targetX + distanceFromCenter * cos(currentAngle);
    y = targetY + distanceFromCenter * sin(currentAngle);

    // Update rotation to always point outward
    rotation = stuckAngle + targetRotation + 90.0f;
}