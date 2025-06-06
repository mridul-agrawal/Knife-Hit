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
    extern float SCALE_X;
    extern float SCALE_Y;
    extern float UNIFORM_SCALE;

    // Scaling functions
    void setScreenDimensions(float width, float height);
    float scaleX(float value);
    float scaleY(float value);
    float scaleUniform(float value);
    float scaleFont(float baseSize);

    // === REFERENCE TARGET PROPERTIES ===
    const float REF_TARGET_RADIUS = 280.0f;
    const float REF_TARGET_X_RATIO = 0.5f;
    const float REF_TARGET_Y_RATIO = 0.3f;

    // Target rotation speeds (degrees per second)
    const float BASE_ROTATION_SPEED = 500.0f;
    const float ROTATION_SPEED_INCREMENT = 45.0f;
    const int LEVEL_FOR_REVERSE_ROTATION = 3;

    // === REFERENCE KNIFE PROPERTIES ===
    const float REF_KNIFE_WIDTH = 24.0f;
    const float REF_KNIFE_LENGTH = 160.0f;

    // Knife positioning
    const float REF_KNIFE_START_X_RATIO = 0.5f;
    const float REF_KNIFE_START_Y_RATIO = 0.85f;

    // Knife physics
    const float KNIFE_SPEED_RATIO = 2.4f;

    // Knife collision and positioning offsets
    const float REF_KNIFE_TIP_OFFSET = 0.0f;
    const float REF_KNIFE_HANDLE_OFFSET = 50.0f;

    // === LEVEL PROGRESSION ===
    const int KNIVES_PER_LEVEL = 6;
    const int MAX_PRE_STUCK_KNIVES = 4;
    const int POINTS_PER_KNIFE = 10;
    const int LEVEL_COMPLETE_BONUS = 50;

    // === REFERENCE UI POSITIONING ===
    const float REF_UI_MARGIN = 60.0f;
    const float REF_UI_LINE_HEIGHT = 80.0f;
    const float REF_UI_TOP_MARGIN = 80.0f;

    // Knife indicator positioning
    const float REF_KNIFE_INDICATOR_Y_RATIO = 0.875f;
    const float REF_KNIFE_INDICATOR_SPACING = 80.0f;
    const float KNIFE_INDICATOR_SCALE = 0.7f;

    // === GAME TIMING ===
    const float COLLISION_PAUSE_DURATION = 2.5f;

    // === FONT SIZES ===
    const int REF_TITLE_FONT_SIZE = 128;
    const int REF_UI_FONT_SIZE = 64;
    const int REF_SCORE_FONT_SIZE = 80;

    // === DYNAMIC GETTERS ===
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

    // Colors namespace (unchanged)
    namespace Colors {
        struct Color {
            unsigned char r, g, b, a;
            Color(unsigned char r = 255, unsigned char g = 255,
                unsigned char b = 255, unsigned char a = 255)
                : r(r), g(g), b(b), a(a) {}
        };

        static const Color BACKGROUND_TOP(22, 160, 133);
        static const Color BACKGROUND_BOTTOM(15, 111, 92);
        static const Color BACKGROUND_DARK(28, 46, 56);

        static const Color WHITE(255, 255, 255);
        static const Color BLACK(0, 0, 0);
        static const Color RED(255, 0, 0);
        static const Color GREEN(76, 217, 100);
        static const Color YELLOW(255, 204, 0);
        static const Color ORANGE(255, 87, 51);

        static const Color WOOD_LIGHT(218, 165, 105);
        static const Color WOOD(185, 135, 85);
        static const Color WOOD_DARK(139, 90, 43);
        static const Color WOOD_RING(110, 70, 35);

        static const Color KNIFE_HANDLE(101, 67, 33);
        static const Color KNIFE_BLADE(192, 192, 192);
        static const Color KNIFE_BLADE_EDGE(160, 160, 160);

        static const Color UI_TEXT(255, 255, 255);
        static const Color UI_SCORE(255, 204, 0);
    }
}