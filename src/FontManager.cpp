#include "../include/FontManager.hpp"
#include "../include/GameConstants.hpp"
#include <iostream>

bool FontManager::initialize() {
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed!" << std::endl;
        return false;
    }

    // Calculate scaled font sizes
    int titleSize = static_cast<int>(GameConstants::scaleFont(GameConstants::REF_TITLE_FONT_SIZE));
    int uiSize = static_cast<int>(GameConstants::scaleFont(GameConstants::REF_UI_FONT_SIZE));
    int scoreSize = static_cast<int>(GameConstants::scaleFont(GameConstants::REF_SCORE_FONT_SIZE));

    std::cout << "Loading fonts with scaled sizes:" << std::endl;
    std::cout << "Title: " << titleSize << "px (ref: " << GameConstants::REF_TITLE_FONT_SIZE << "px)" << std::endl;
    std::cout << "UI: " << uiSize << "px (ref: " << GameConstants::REF_UI_FONT_SIZE << "px)" << std::endl;
    std::cout << "Score: " << scoreSize << "px (ref: " << GameConstants::REF_SCORE_FONT_SIZE << "px)" << std::endl;

    // Try primary paths first, then fallback paths
    if (!loadFont(TITLE_FONT, "/assets/fonts/Fredoka-Bold.ttf", titleSize) &&
        !loadFont(TITLE_FONT, "Fredoka-Bold.ttf", titleSize)) {
        std::cerr << "Failed to load title font!" << std::endl;
        return false;
    }

    if (!loadFont(UI_FONT, "/assets/fonts/Rubik-Medium.ttf", uiSize) &&
        !loadFont(UI_FONT, "Rubik-Medium.ttf", uiSize)) {
        std::cerr << "Failed to load UI font!" << std::endl;
        return false;
    }

    if (!loadFont(SCORE_FONT, "/assets/fonts/Rubik-Bold.ttf", scoreSize) &&
        !loadFont(SCORE_FONT, "Rubik-Bold.ttf", scoreSize)) {
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
        std::cerr << "Failed to load font " << path << " with size " << size << std::endl;
        return false;
    }

    fonts[name] = font;
    std::cout << "Successfully loaded font: " << name << " (" << path << ") at size " << size << std::endl;
    return true;
}

TTF_Font* FontManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }
    return nullptr;
}