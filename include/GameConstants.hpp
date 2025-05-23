#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GameConstants {
    // Screen dimensions
    const int SCREEN_WIDTH = 400;
    const int SCREEN_HEIGHT = 600;

    // Game mechanics
    const int KNIVES_PER_LEVEL = 8;
    const float KNIFE_SPEED = 800.0f;
    const float TARGET_RADIUS = 80.0f;

    // Knife dimensions
    const float KNIFE_LENGTH = 60.0f;
    const float KNIFE_WIDTH = 8.0f;

    // Game positioning - calculated explicitly
    const float KNIFE_START_Y = 450.0f; // SCREEN_HEIGHT - 150
    const float TARGET_Y = 300.0f;
    const float TARGET_X = 200.0f;      // SCREEN_WIDTH / 2

    // Collision detection
    const float COLLISION_THRESHOLD = 25.0f;
    const float TARGET_HIT_DISTANCE = 110.0f; // TARGET_RADIUS + 30

    // Scoring
    const int POINTS_PER_KNIFE = 10;

    // Level progression
    const float BASE_ROTATION_SPEED = 30.0f;
    const float ROTATION_SPEED_INCREMENT = 15.0f;
    const int LEVEL_FOR_REVERSE_ROTATION = 3;
    const int MAX_PRE_STUCK_KNIVES = 4;

    // UI positioning
    const int UI_MARGIN = 20;
    const int UI_LINE_HEIGHT = 30;
    const int CHARACTER_WIDTH = 20;
    const int CHARACTER_HEIGHT = 20;

    // Colors namespace
    namespace Colors {
        struct Color {
            unsigned char r, g, b, a;
            Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
                : r(red), g(green), b(blue), a(alpha) {}
        };

        // Color instances - defined as static to avoid multiple definition
        static const Color WHITE(255, 255, 255, 255);
        static const Color BLACK(0, 0, 0, 255);
        static const Color DARK_BLUE(20, 20, 40, 255);
        static const Color RED(255, 0, 0, 255);
        static const Color GREEN(0, 255, 0, 255);
        static const Color BROWN(139, 69, 19, 255);
        static const Color SILVER(200, 200, 200, 255);
        static const Color DARK_SILVER(150, 150, 150, 255);
        static const Color WOOD(160, 82, 45, 255);
        static const Color DARK_WOOD(80, 40, 20, 255);
    }
}