#include "../include/FontManager.hpp"
#include <iostream>

bool FontManager::initialize() {
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed!" << std::endl;
        return false;
    }

    // Load all required fonts with appropriate sizes
    // Title font - Bold and chunky for KNIFE HIT, GAME OVER, etc.
    if (!loadFont(TITLE_FONT, "assets/fonts/Fredoka-Bold.ttf", 64)) {
        std::cerr << "Failed to load title font!" << std::endl;
        return false;
    }

    // UI font - Medium weight for TAP TO PLAY, STAGE X, etc.
    if (!loadFont(UI_FONT, "assets/fonts/Rubik-Medium.ttf", 32)) {
        std::cerr << "Failed to load UI font!" << std::endl;
        return false;
    }

    // Score font - Bold for numbers
    if (!loadFont(SCORE_FONT, "assets/fonts/Rubik-Bold.ttf", 40)) {
        std::cerr << "Failed to load score font!" << std::endl;
        return false;
    }

    return true;
}

void FontManager::cleanup() {
    for (auto& pair : fonts) {
        if (pair.second) {
            TTF_CloseFont(pair.second);
        }
    }
    fonts.clear();
    TTF_Quit();
}

bool FontManager::loadFont(const std::string& name, const std::string& path, int size) {
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cerr << "Failed to load font " << path << std::endl;
        return false;
    }

    fonts[name] = font;
    return true;
}

TTF_Font* FontManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }
    return nullptr;
}