#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <SDL3/SDL.h>
#include "GameConstants.hpp"

class Knife {
public:
    Knife();
    void update(float deltaTime);
    void reset();
    void stick(float targetX, float targetY, float targetRotation);
    void updateStuckPosition(float targetX, float targetY, float targetRotation);  // NEW

    float getX() const { return x; }
    float getY() const { return y; }
    bool isKnifeStuck() const { return isStuck; }
    bool isKnifeActive() const { return isActive; }
    float getStuckAngle() const { return stuckAngle; }
    float getDistanceFromCenter() const { return distanceFromCenter; }
    void setVelocityY(float vel) { velY = vel; }
    void setActive(bool active) { isActive = active; }

    // NEW: Get rotation for rendering stuck knives
    float getRotation() const { return rotation; }
    float getHandleX() const;           // Get handle center X position
    float getHandleY() const;           // Get handle center Y position  
    float getBladeX() const;            // Get blade center X position
    float getBladeY() const;            // Get blade center Y position
    float getDistanceFromTargetCenter(float targetX, float targetY) const;

private:
    float x, y;
    float velY;
    bool isStuck;
    bool isActive;
    float stuckAngle;
    float distanceFromCenter;
    float rotation;  // NEW: For rotating stuck knives
};