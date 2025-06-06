#include "../include/CollisionDetector.hpp"
#include "../include/Knife.hpp"
#include "../include/Target.hpp"
#include <iostream>

CollisionResult CollisionDetector::checkKnifeTargetCollision(const Knife& knife, const Target& target) {
    if (!knife.isKnifeActive() || knife.isKnifeStuck()) {
        return CollisionResult(false);
    }

    // Get knife tip position (front of the knife)
    float knifeX = knife.getX();
    float knifeY = knife.getY() - (GameConstants::getKnifeLength() / 2.0f);  // Tip is at front

    // Get target info
    float targetX = target.getX();
    float targetY = target.getY();
    float targetRadius = target.getRadius();

    // Calculate distance from knife tip to target center
    float distanceToCenter = getDistanceBetweenPoints(knifeX, knifeY, targetX, targetY);

    // Check if knife tip has reached the target's edge
    float threshold = GameConstants::scaleUniform(KNIFE_TARGET_COLLISION_THRESHOLD);
    if (distanceToCenter <= targetRadius + threshold) {
        // Calculate collision point on target edge
        CollisionPoint collisionPoint = getTargetCollisionPoint(knife, target);
        return CollisionResult(true, collisionPoint);
    }

    return CollisionResult(false);
}

bool CollisionDetector::checkKnifeKnifeCollision(const Knife& incomingKnife,
    const std::vector<Knife>& stuckKnives,
    const Target& target) {
    if (stuckKnives.empty()) {
        return false;
    }

    // Get incoming knife's collision point
    CollisionPoint incomingPoint = getTargetCollisionPoint(incomingKnife, target);

    std::cout << "\n=== KNIFE COLLISION CHECK ===" << std::endl;
    std::cout << "Incoming knife angle: " << incomingPoint.angle << "°" << std::endl;
    std::cout << "Checking against " << stuckKnives.size() << " stuck knives" << std::endl;

    // Check against each stuck knife
    for (size_t i = 0; i < stuckKnives.size(); i++) {
        const Knife& stuckKnife = stuckKnives[i];

        if (!stuckKnife.isKnifeStuck()) {
            continue;
        }

        // Get stuck knife angle
        float stuckAngle = stuckKnife.getStuckAngle();

        // Calculate angular difference
        float angularDiff = getAngularDistance(incomingPoint.angle, stuckAngle);

        // Calculate physical separation at knife handle radius
        float handleRadius = target.getRadius() + GameConstants::getKnifeHandleOffset();
        float physicalSeparation = getPhysicalSeparationAtRadius(angularDiff, handleRadius);

        // Get scaled minimum separation threshold
        float minSeparation = GameConstants::scaleUniform(KNIFE_KNIFE_MIN_SEPARATION);

        std::cout << "Stuck knife " << i << ": angle=" << stuckAngle
            << "°, angular_diff=" << angularDiff
            << "°, physical_sep=" << physicalSeparation
            << "px, min_required=" << minSeparation << "px" << std::endl;

        if (physicalSeparation < minSeparation) {
            std::cout << "🔴 COLLISION DETECTED!" << std::endl;
            return true;
        }
    }

    std::cout << "✅ No collisions detected" << std::endl;
    return false;
}

CollisionPoint CollisionDetector::getTargetCollisionPoint(const Knife& knife, const Target& target) {
    float knifeX = knife.getX();
    float knifeY = knife.getY();
    float targetX = target.getX();
    float targetY = target.getY();

    // Calculate angle from target center to knife
    float absoluteAngle = getAngleBetweenPoints(targetX, targetY, knifeX, knifeY);

    // Convert to relative angle (relative to target's current rotation)
    float relativeAngle = absoluteAngle - target.getRotation();
    relativeAngle = normalizeAngle(relativeAngle);

    // Calculate collision point on target edge
    float collisionX = targetX + target.getRadius() * cos(absoluteAngle * M_PI / 180.0f);
    float collisionY = targetY + target.getRadius() * sin(absoluteAngle * M_PI / 180.0f);

    return CollisionPoint(collisionX, collisionY, relativeAngle);
}

float CollisionDetector::getDistanceBetweenPoints(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

float CollisionDetector::getAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    return atan2(y2 - y1, x2 - x1) * 180.0f / M_PI;
}

float CollisionDetector::normalizeAngle(float angle) {
    while (angle < 0) angle += 360.0f;
    while (angle >= 360.0f) angle -= 360.0f;
    return angle;
}

float CollisionDetector::getAngularDistance(float angle1, float angle2) {
    float diff = abs(angle1 - angle2);
    if (diff > 180.0f) {
        diff = 360.0f - diff;
    }
    return diff;
}

float CollisionDetector::getPhysicalSeparationAtRadius(float angularDiff, float radius) {
    // Convert angular difference to arc length (physical distance)
    return (angularDiff * M_PI / 180.0f) * radius;
}