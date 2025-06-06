#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace GameConstants {
    // Reference dimensions (1080x1920 as specified)
    const float REFERENCE_WIDTH = 1080.0f;
    const float REFERENCE_HEIGHT = 1920.0f;
    const float REFERENCE_ASPECT_RATIO = REFERENCE_WIDTH / REFERENCE_HEIGHT;

    // Current screen dimensions (will be set dynamically)
    extern float CURRENT_WIDTH;
    extern float CURRENT_HEIGHT;
    extern float SCALE_X;  // Horizontal scaling factor
    extern float SCALE_Y;  // Vertical scaling factor
    extern float UNIFORM_SCALE;  // Uniform scaling factor (minimum of SCALE_X and SCALE_Y)

    // Scaling functions
    void setScreenDimensions(float width, float height);
    float scaleX(float value);  // Scale horizontal values
    float scaleY(float value);  // Scale vertical values
    float scaleUniform(float value);  // Scale using uniform factor
    float scaleFont(float baseSize);  // Scale font sizes

    // === REFERENCE TARGET PROPERTIES ===
    // Target dimensions and positioning (based on reference resolution)
    const float REF_TARGET_RADIUS = 280.0f;  // Target radius in reference resolution
    const float REF_TARGET_X_RATIO = 0.5f;   // Target X position as ratio of screen width (center)
    const float REF_TARGET_Y_RATIO = 0.3f;   // Target Y position as ratio of screen height (30% from top)

    // Target rotation speeds (degrees per second)
    const float BASE_ROTATION_SPEED = 90.0f;       // Base rotation speed for level 1
    const float ROTATION_SPEED_INCREMENT = 45.0f;  // Speed increase per level
    const int LEVEL_FOR_REVERSE_ROTATION = 3;      // Level when rotation can reverse direction

    // === REFERENCE KNIFE PROPERTIES ===
    // Knife dimensions (based on reference resolution)
    const float REF_KNIFE_WIDTH = 24.0f;    // Knife sprite width
    const float REF_KNIFE_LENGTH = 160.0f;  // Knife sprite length

    // Knife positioning
    const float REF_KNIFE_START_X_RATIO = 0.5f;   // Starting X position as ratio (center)
    const float REF_KNIFE_START_Y_RATIO = 0.85f;  // Starting Y position as ratio (85% from top)

    // Knife physics
    const float KNIFE_SPEED_RATIO = 2.4f;  // Knife speed as ratio of screen height per second

    // Knife collision and positioning offsets
    const float REF_KNIFE_TIP_OFFSET = 0.0f;      // How much knife penetrates into target
    const float REF_KNIFE_HANDLE_OFFSET = 50.0f;  // Distance from target edge to knife handle

    // === LEVEL PROGRESSION ===
    const int KNIVES_PER_LEVEL = 6;               // Number of knives per level
    const int MAX_PRE_STUCK_KNIVES = 4;           // Maximum pre-stuck knives in higher levels
    const int POINTS_PER_KNIFE = 10;              // Points awarded per successful knife throw
    const int LEVEL_COMPLETE_BONUS = 50;          // Bonus points for completing a level

    // === REFERENCE UI POSITIONING ===
    // UI margins and spacing (based on reference resolution)
    const float REF_UI_MARGIN = 60.0f;           // General UI margin from screen edges
    const float REF_UI_LINE_HEIGHT = 80.0f;      // Height between UI text lines
    const float REF_UI_TOP_MARGIN = 80.0f;       // Top margin for UI elements

    // Knife indicator positioning
    const float REF_KNIFE_INDICATOR_Y_RATIO = 0.875f;  // Y position as ratio from top (87.5%)
    const float REF_KNIFE_INDICATOR_SPACING = 80.0f;   // Spacing between knife indicators
    const float KNIFE_INDICATOR_SCALE = 0.7f;          // Scale factor for indicator knives

    // === COLLISION DETECTION ===
    // Collision thresholds (will be scaled dynamically)
    // NOTE: These box-based collision constants are currently unused.
    // The system uses angular collision detection instead.
    // const float REF_KNIFE_HANDLE_WIDTH = 30.0f;        // Width of knife handle for collision (UNUSED)
    // const float REF_KNIFE_HANDLE_LENGTH = 50.0f;       // Length of knife handle for collision (UNUSED)
    // const float REF_HANDLE_COLLISION_BUFFER = 10.0f;   // Extra buffer for handle collision (UNUSED)
    // const float REF_BLADE_WIDTH = 10.0f;               // Width of knife blade (UNUSED)

    const float REF_KNIFE_TARGET_HIT_THRESHOLD = 10.0f; // Distance threshold for target hits
    const float REF_SPATIAL_COLLISION_THRESHOLD = 50.0f; // Minimum separation between knife handles (angular system)

    // === GAME TIMING ===
    const float COLLISION_PAUSE_DURATION = 2.5f;  // Duration of collision pause in seconds

    // === FONT SIZES (Reference sizes) ===
    const int REF_TITLE_FONT_SIZE = 128;   // Title font size in reference resolution
    const int REF_UI_FONT_SIZE = 64;       // UI font size in reference resolution
    const int REF_SCORE_FONT_SIZE = 80;    // Score font size in reference resolution

    // === DYNAMIC GETTERS (Use these in game logic) ===
    // Target properties
    float getTargetRadius();
    float getTargetX();
    float getTargetY();

    // Knife properties
    float getKnifeWidth();
    float getKnifeLength();
    float getKnifeSpeed();
    float getKnifeStartX();
    float getKnifeStartY();
    float getKnifeTipOffset();
    float getKnifeHandleOffset();

    // UI positioning
    float getUIMargin();
    float getUILineHeight();
    float getUITopMargin();
    float getKnifeIndicatorY();
    float getKnifeIndicatorSpacing();

    // Collision detection (only the ones actually used)
    float getKnifeTargetHitThreshold();
    float getSpatialCollisionThreshold();

    // NOTE: These getters are commented out as the corresponding constants are unused:
    // float getKnifeHandleWidth();
    // float getKnifeHandleLength(); 
    // float getHandleCollisionBuffer();
    // float getBladeWidth();

    // Colors namespace (unchanged as colors don't need scaling)
    namespace Colors {
        struct Color {
            unsigned char r, g, b, a;
            Color(unsigned char r = 255, unsigned char g = 255,
                unsigned char b = 255, unsigned char a = 255)
                : r(r), g(g), b(b), a(a) {}
        };

        // Game colors
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