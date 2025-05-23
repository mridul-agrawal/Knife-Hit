#pragma once

#include <SDL3/SDL.h>
#include "GameConstants.hpp"

class Knife {
public:
    Knife();
    void update(float deltaTime);
    void reset();
    void stick(float targetX, float targetY, float targetRotation);

    float getX() const { return x; }
    float getY() const { return y; }
    bool isKnifeStuck() const { return isStuck; }
    bool isKnifeActive() const { return isActive; }
    float getStuckAngle() const { return stuckAngle; }
    float getDistanceFromCenter() const { return distanceFromCenter; }
    void setVelocityY(float vel) { velY = vel; }
    void setActive(bool active) { isActive = active; }

private:
    float x, y;
    float velY;
    bool isStuck;
    bool isActive;
    float stuckAngle;
    float distanceFromCenter;
};