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
    distanceFromCenter = GameConstants::TARGET_RADIUS;

    // Position knife exactly at the edge of target
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;
    float adjustedDistance = distanceFromCenter - GameConstants::KNIFE_IMAGE_TIP_OFFSET;
    x = targetX + adjustedDistance * cos(currentAngle);
    y = targetY + adjustedDistance * sin(currentAngle);

    // Keep visual appearance identical (no sprite rotation)
    rotation = 0.0f;
}

// NEW: Update position of stuck knife as target rotates
void Knife::updateStuckPosition(float targetX, float targetY, float targetRotation) {
    if (!isStuck) return;

    // Calculate current angle based on target rotation
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;

    // Update position based on distance and angle
    float adjustedDistance = distanceFromCenter - GameConstants::KNIFE_IMAGE_TIP_OFFSET;
    x = targetX + adjustedDistance * cos(currentAngle);
    y = targetY + adjustedDistance * sin(currentAngle);

    // Update rotation to always point outward
    rotation = stuckAngle + targetRotation - 90.0f;
}

float Knife::getHandleX() const {
    if (!isStuck) return x;

    // Handle is at the outer end of the stuck knife
    float handleOffset = GameConstants::KNIFE_IMAGE_HANDLE_OFFSET;
    float rad = (rotation - 90.0f) * M_PI / 180.0f;  // -90 because handle points outward
    return x + handleOffset * cos(rad);
}

float Knife::getHandleY() const {
    if (!isStuck) return y;

    // Handle is at the outer end of the stuck knife  
    float handleOffset = GameConstants::KNIFE_IMAGE_HANDLE_OFFSET;
    float rad = (rotation - 90.0f) * M_PI / 180.0f;
    return y + handleOffset * sin(rad);
}

float Knife::getBladeX() const {
    if (!isStuck) return x;

    // Blade tip points toward target center
    float bladeOffset = GameConstants::KNIFE_IMAGE_TIP_OFFSET;
    float rad = (rotation + 90.0f) * M_PI / 180.0f;  // +90 because blade points inward
    return x + bladeOffset * cos(rad);
}

float Knife::getBladeY() const {
    if (!isStuck) return y;

    // Blade tip points toward target center
    float bladeOffset = GameConstants::KNIFE_IMAGE_TIP_OFFSET;
    float rad = (rotation + 90.0f) * M_PI / 180.0f;
    return y + bladeOffset * sin(rad);
}

float Knife::getDistanceFromTargetCenter(float targetX, float targetY) const {
    float dx = x - targetX;
    float dy = y - targetY;
    return sqrt(dx * dx + dy * dy);
}