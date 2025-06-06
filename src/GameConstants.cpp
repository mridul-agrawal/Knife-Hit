#include "../include/GameConstants.hpp"
#include <algorithm>

namespace GameConstants {
    // Current screen dimensions (initialized to reference values)
    float CURRENT_WIDTH = REFERENCE_WIDTH;
    float CURRENT_HEIGHT = REFERENCE_HEIGHT;
    float SCALE_X = 1.0f;
    float SCALE_Y = 1.0f;
    float UNIFORM_SCALE = 1.0f;

    void setScreenDimensions(float width, float height) {
        CURRENT_WIDTH = width;
        CURRENT_HEIGHT = height;

        SCALE_X = width / REFERENCE_WIDTH;
        SCALE_Y = height / REFERENCE_HEIGHT;

        UNIFORM_SCALE = std::min(SCALE_X, SCALE_Y);
    }

    float scaleX(float value) {
        return value * SCALE_X;
    }

    float scaleY(float value) {
        return value * SCALE_Y;
    }

    float scaleUniform(float value) {
        return value * UNIFORM_SCALE;
    }

    float scaleFont(float baseSize) {
        return baseSize * UNIFORM_SCALE;
    }

    // === DYNAMIC GETTERS ===

    // Target properties
    float getTargetRadius() {
        return scaleUniform(REF_TARGET_RADIUS);
    }

    float getTargetX() {
        return CURRENT_WIDTH * REF_TARGET_X_RATIO;
    }

    float getTargetY() {
        return CURRENT_HEIGHT * REF_TARGET_Y_RATIO;
    }

    // Knife properties
    float getKnifeWidth() {
        return scaleUniform(REF_KNIFE_WIDTH);
    }

    float getKnifeLength() {
        return scaleUniform(REF_KNIFE_LENGTH);
    }

    float getKnifeSpeed() {
        return CURRENT_HEIGHT * KNIFE_SPEED_RATIO;
    }

    float getKnifeStartX() {
        return CURRENT_WIDTH * REF_KNIFE_START_X_RATIO;
    }

    float getKnifeStartY() {
        return CURRENT_HEIGHT * REF_KNIFE_START_Y_RATIO;
    }

    float getKnifeTipOffset() {
        return scaleUniform(REF_KNIFE_TIP_OFFSET);
    }

    float getKnifeHandleOffset() {
        return scaleUniform(REF_KNIFE_HANDLE_OFFSET);
    }

    // UI positioning
    float getUIMargin() {
        return scaleUniform(REF_UI_MARGIN);
    }

    float getUILineHeight() {
        return scaleUniform(REF_UI_LINE_HEIGHT);
    }

    float getUITopMargin() {
        return scaleUniform(REF_UI_TOP_MARGIN);
    }

    float getKnifeIndicatorY() {
        return CURRENT_HEIGHT * REF_KNIFE_INDICATOR_Y_RATIO;
    }

    float getKnifeIndicatorSpacing() {
        return scaleUniform(REF_KNIFE_INDICATOR_SPACING);
    }
}