#pragma once
#include <vector>
#include <cmath>
#include "GameConstants.hpp"

// Forward declarations
class Knife;
class Target;

struct CollisionPoint {
    float x, y;
    float angle;  // Angle relative to target center (0-360 degrees)

    CollisionPoint(float x = 0, float y = 0, float angle = 0)
        : x(x), y(y), angle(angle) {}
};

struct CollisionResult {
    bool hasCollision;
    CollisionPoint point;

    CollisionResult(bool collision = false, const CollisionPoint& pt = CollisionPoint())
        : hasCollision(collision), point(pt) {}
};

class CollisionDetector {
public:
    // Knife vs Target collision detection
    static CollisionResult checkKnifeTargetCollision(const Knife& knife, const Target& target);

    // Knife vs Knife collision detection
    static bool checkKnifeKnifeCollision(const Knife& incomingKnife,
        const std::vector<Knife>& stuckKnives,
        const Target& target);

    // Helper method to get collision point on target edge
    static CollisionPoint getTargetCollisionPoint(const Knife& knife, const Target& target);

private:
    // Helper methods for clean separation of concerns
    static float getDistanceBetweenPoints(float x1, float y1, float x2, float y2);
    static float getAngleBetweenPoints(float x1, float y1, float x2, float y2);
    static float normalizeAngle(float angle);
    static float getAngularDistance(float angle1, float angle2);
    static float getPhysicalSeparationAtRadius(float angularDiff, float radius);

    // Collision thresholds (scaled automatically)
    static constexpr float KNIFE_TARGET_COLLISION_THRESHOLD = 5.0f;  // How close knife tip needs to be to target edge
    static constexpr float KNIFE_KNIFE_MIN_SEPARATION = 45.0f;       // Minimum physical separation between knife handles
};