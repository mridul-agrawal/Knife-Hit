#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GameConstants {
    // Screen dimensions
    constexpr int SCREEN_WIDTH = 400;
    constexpr int SCREEN_HEIGHT = 600;

    // Game mechanics
    constexpr int KNIVES_PER_LEVEL = 8;
    constexpr float KNIFE_SPEED = 800.0f;
    constexpr float TARGET_RADIUS = 80.0f;

    // Knife dimensions
    constexpr float KNIFE_LENGTH = 60.0f;
    constexpr float KNIFE_WIDTH = 8.0f;

    // Game positioning
    constexpr float KNIFE_START_Y = SCREEN_HEIGHT - 150.0f;
    constexpr float TARGET_Y = 300.0f;
    constexpr float TARGET_X = SCREEN_WIDTH / 2.0f;

    // Collision detection
    constexpr float COLLISION_THRESHOLD = 25.0f;
    constexpr float TARGET_HIT_DISTANCE = TARGET_RADIUS + 30.0f;

    // Scoring
    constexpr int POINTS_PER_KNIFE = 10;

    // Level progression
    constexpr float BASE_ROTATION_SPEED = 30.0f;
    constexpr float ROTATION_SPEED_INCREMENT = 15.0f;
    constexpr int LEVEL_FOR_REVERSE_ROTATION = 3;
    constexpr int MAX_PRE_STUCK_KNIVES = 4;

    // UI positioning
    constexpr int UI_MARGIN = 20;
    constexpr int UI_LINE_HEIGHT = 30;
    constexpr int CHARACTER_WIDTH = 20;
    constexpr int CHARACTER_HEIGHT = 20;

    // Colors (RGB values)
    namespace Colors {
        constexpr struct Color {
            unsigned char r, g, b, a;
            constexpr Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
                : r(red), g(green), b(blue), a(alpha) {}
        }
        WHITE{255, 255, 255},
            BLACK{ 0, 0, 0 },
            DARK_BLUE{ 20, 20, 40 },
            RED{ 255, 0, 0 },
            GREEN{ 0, 255, 0 },
            BROWN{ 139, 69, 19 },
            SILVER{ 200, 200, 200 },
            DARK_SILVER{ 150, 150, 150 },
            WOOD{ 160, 82, 45 },
            DARK_WOOD{ 80, 40, 20 };
    }
}