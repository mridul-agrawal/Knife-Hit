#include "../include/Knife.hpp"
#include <cmath>

Knife::Knife()
    : x(GameConstants::getTargetX())
    , y(GameConstants::getKnifeStartY())
    , velY(0)
    , isStuck(false)
    , isActive(true)
    , stuckAngle(0)
    , distanceFromCenter(0)
    , rotation(0) {
}

void Knife::update(float deltaTime) {
    if (isActive && !isStuck) {
        y += velY * deltaTime;
    }
}

void Knife::reset() {
    x = GameConstants::getTargetX();
    y = GameConstants::getKnifeStartY();
    velY = 0;
    isStuck = false;
    isActive = true;
    stuckAngle = 0;
    distanceFromCenter = 0;
    rotation = 0;
}

void Knife::stick(float targetX, float targetY, float targetRotation) {
    isStuck = true;
    velY = 0;

    // Calculate angle relative to target center  
    float angle = atan2(y - targetY, x - targetX) * 180.0f / M_PI;
    angle -= targetRotation; // Make relative to target rotation
    if (angle < 0) angle += 360;

    stuckAngle = angle;
    distanceFromCenter = GameConstants::getTargetRadius();

    // Position knife exactly at the edge of target
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;
    float adjustedDistance = distanceFromCenter - GameConstants::getKnifeTipOffset();
    x = targetX + adjustedDistance * cos(currentAngle);
    y = targetY + adjustedDistance * sin(currentAngle);

    // Keep visual appearance identical (no sprite rotation)
    rotation = 0.0f;
}

void Knife::updateStuckPosition(float targetX, float targetY, float targetRotation) {
    if (!isStuck) return;

    // Calculate current angle based on target rotation
    float currentAngle = (stuckAngle + targetRotation) * M_PI / 180.0f;

    // Update position based on distance and angle
    float adjustedDistance = distanceFromCenter - GameConstants::getKnifeTipOffset();
    x = targetX + adjustedDistance * cos(currentAngle);
    y = targetY + adjustedDistance * sin(currentAngle);

    // Update rotation to always point outward
    rotation = stuckAngle + targetRotation - 90.0f;
}

float Knife::getHandleX() const {
    if (!isStuck) return x;

    // Handle is at the outer end of the stuck knife
    float handleOffset = GameConstants::getKnifeHandleOffset();
    float rad = (rotation - 90.0f) * M_PI / 180.0f;  // -90 because handle points outward
    return x + handleOffset * cos(rad);
}

float Knife::getHandleY() const {
    if (!isStuck) return y;

    // Handle is at the outer end of the stuck knife  
    float handleOffset = GameConstants::getKnifeHandleOffset();
    float rad = (rotation - 90.0f) * M_PI / 180.0f;
    return y + handleOffset * sin(rad);
}

float Knife::getBladeX() const {
    if (!isStuck) return x;

    // Blade tip points toward target center
    float bladeOffset = GameConstants::getKnifeTipOffset();
    float rad = (rotation + 90.0f) * M_PI / 180.0f;  // +90 because blade points inward
    return x + bladeOffset * cos(rad);
}

float Knife::getBladeY() const {
    if (!isStuck) return y;

    // Blade tip points toward target center
    float bladeOffset = GameConstants::getKnifeTipOffset();
    float rad = (rotation + 90.0f) * M_PI / 180.0f;
    return y + bladeOffset * sin(rad);
}

float Knife::getDistanceFromTargetCenter(float targetX, float targetY) const {
    float dx = x - targetX;
    float dy = y - targetY;
    return sqrt(dx * dx + dy * dy);
}