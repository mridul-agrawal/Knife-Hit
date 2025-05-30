#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GameConstants {
    // Screen dimensions - more reasonable size for desktop
    const int SCREEN_WIDTH = 450;   // Reduced from 720
    const int SCREEN_HEIGHT = 800;  // Reduced from 1280

    // Reference dimensions for scaling
    const float ASPECT_RATIO = 9.0f / 16.0f;
    const int REFERENCE_WIDTH = 450;
    const int REFERENCE_HEIGHT = 800;

    // Target properties - adjusted for image-based target
    const float TARGET_RADIUS = 140.0f;  // Keep same radius for collision detection
    const float TARGET_X = SCREEN_WIDTH / 2.0f;
    const float TARGET_Y = SCREEN_HEIGHT * 0.3f;  // 30% from top
    const float TARGET_ROTATION_SPEED = 90.0f;  // INCREASED: 3x faster (was 30.0f)

    // Knife properties
    const float KNIFE_WIDTH = 12.0f;   // Slightly wider
    const float KNIFE_LENGTH = 80.0f;  // Longer knife
    const float KNIFE_SPEED = 1200.0f; // Faster throw
    const float KNIFE_START_X = SCREEN_WIDTH / 2.0f;
    const float KNIFE_START_Y = SCREEN_HEIGHT * 0.85f;  // 85% from top
    const float KNIFE_HIT_DISTANCE = TARGET_RADIUS + 35.0f;

    // Level progression
    const int KNIVES_PER_LEVEL = 6;  // This was missing!
    const float BASE_ROTATION_SPEED = 90.0f;  // INCREASED: 3x faster
    const float ROTATION_SPEED_INCREMENT = 45.0f;  // INCREASED: 3x faster increment
    const int LEVEL_FOR_REVERSE_ROTATION = 3;  // Level when rotation can reverse
    const int MAX_PRE_STUCK_KNIVES = 4;  // Max pre-stuck knives in higher levels
    const int POINTS_PER_KNIFE = 10;
    const int LEVEL_COMPLETE_BONUS = 50;

    // UI positioning - properly centered
    const int UI_MARGIN = 30;
    const int UI_LINE_HEIGHT = 35;
    const int UI_TOP_MARGIN = 40;

    // Knife indicators positioning
    const float KNIFE_INDICATOR_Y = SCREEN_HEIGHT - 120.0f;  // Adjusted position
    const float KNIFE_INDICATOR_SPACING = 40.0f;  // More spacing between indicators
    const float KNIFE_INDICATOR_SCALE = 0.7f;  // Slightly larger indicators

    // Add new knife image constants:
    const float KNIFE_IMAGE_TIP_OFFSET = 0.0f;    // More penetration into target
    const float KNIFE_IMAGE_HANDLE_OFFSET = 25.0f; // Less handle showing

    // Collision detection - TUNED FOR IMAGE TARGET
    const float KNIFE_HANDLE_WIDTH = 15.0f;       // Measure your sprite's handle width
    const float KNIFE_HANDLE_LENGTH = 25.0f;      // Measure your sprite's handle length  
    const float HANDLE_COLLISION_BUFFER = 5.0f;   // Reduce for tighter collision
    const float BLADE_WIDTH = 5.0f;               // Measure your sprite's blade width
    const float KNIFE_TARGET_HIT_THRESHOLD = 5.0f;  // Distance for knife to register hit on target
    
    const float MIN_HANDLE_SEPARATION = KNIFE_HANDLE_WIDTH + HANDLE_COLLISION_BUFFER; // 11px minimum
    const float SPATIAL_COLLISION_THRESHOLD = 25.0f; // Increased for testing
    // const float SPATIAL_COLLISION_THRESHOLD = MIN_HANDLE_SEPARATION; // Use handle separation

    // Target image specific constants
    const float TARGET_IMAGE_SCALE = 1.0f;    // Scale factor if image needs resizing
    const float TARGET_EDGE_TOLERANCE = 5.0f; // Extra tolerance for edge detection with image

    // Colors namespace
    namespace Colors {
        struct Color {
            unsigned char r, g, b, a;
            Color(unsigned char r = 255, unsigned char g = 255,
                unsigned char b = 255, unsigned char a = 255)
                : r(r), g(g), b(b), a(a) {}
        };

        // Game colors matching original
        static const Color BACKGROUND_TOP(22, 160, 133);      // Teal gradient top
        static const Color BACKGROUND_BOTTOM(15, 111, 92);    // Teal gradient bottom
        static const Color BACKGROUND_DARK(28, 46, 56);       // Dark background

        static const Color WHITE(255, 255, 255);
        static const Color BLACK(0, 0, 0);
        static const Color RED(255, 0, 0);
        static const Color GREEN(76, 217, 100);               // Bright green
        static const Color YELLOW(255, 204, 0);               // Gold/yellow
        static const Color ORANGE(255, 87, 51);               // Orange button

        // Wood colors for target (fallback)
        static const Color WOOD_LIGHT(218, 165, 105);         // Light wood
        static const Color WOOD(185, 135, 85);                // Main wood color
        static const Color WOOD_DARK(139, 90, 43);            // Dark wood
        static const Color WOOD_RING(110, 70, 35);            // Ring color

        // Knife colors
        static const Color KNIFE_HANDLE(101, 67, 33);         // Brown handle
        static const Color KNIFE_BLADE(192, 192, 192);        // Silver blade
        static const Color KNIFE_BLADE_EDGE(160, 160, 160);   // Darker silver edge

        // UI colors
        static const Color UI_TEXT(255, 255, 255);
        static const Color UI_SCORE(255, 204, 0);             // Yellow for score
    }
}