#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

class FontManager {
public:
    static FontManager& getInstance() {
        static FontManager instance;
        return instance;
    }

    bool initialize();
    void cleanup();

    // Font loading
    bool loadFont(const std::string& name, const std::string& path, int size);
    TTF_Font* getFont(const std::string& name);

    // Predefined font names
    static constexpr const char* TITLE_FONT = "title";
    static constexpr const char* UI_FONT = "ui";
    static constexpr const char* SCORE_FONT = "score";

private:
    FontManager() = default;
    ~FontManager() { cleanup(); }

    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    std::unordered_map<std::string, TTF_Font*> fonts;
};