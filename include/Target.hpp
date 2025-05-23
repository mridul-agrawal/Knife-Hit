#pragma once

#include <vector>
#include "GameConstants.hpp"

class Target {
public:
    Target();
    void update(float deltaTime);
    void reset(int level);
    void addStuckKnife(float angle, float distance);

    // Add this method for setting position
    void setPosition(float newX, float newY) { x = newX; y = newY; }

    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }
    float getRotation() const { return rotation; }
    const std::vector<float>& getStuckKnifeAngles() const { return stuckKnifeAngles; }
    const std::vector<float>& getStuckKnifeDistances() const { return stuckKnifeDistances; }

private:
    float x, y;
    float radius;
    float rotation;
    float rotationSpeed;
    std::vector<float> stuckKnifeAngles;
    std::vector<float> stuckKnifeDistances;
};