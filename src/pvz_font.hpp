#pragma once

#include <sf3.hpp>
#include "font/sf3_font.hpp"
#include <memory>
#include <string>

namespace PVZ {

// Centralized font manager for the PVZ game
class FontManager {
public:
    static FontManager& instance();

    // Initialize fonts - call after SF3::App::init()
    bool init();

    // Draw text at position with color (UTF-8 strings, supports Chinese)
    void drawText(const std::string& text, float x, float y,
                  SF3::Color color = SF3::Color::White(), float scale = 1.0f);

    // Draw text centered horizontally at x
    void drawTextCentered(const std::string& text, float cx, float y,
                          SF3::Color color = SF3::Color::White(), float scale = 1.0f);

    // Draw text right-aligned at x
    void drawTextRight(const std::string& text, float rightX, float y,
                       SF3::Color color = SF3::Color::White(), float scale = 1.0f);

    // Draw text with shadow
    void drawTextShadow(const std::string& text, float x, float y,
                        SF3::Color color = SF3::Color::White(), float scale = 1.0f);

    // Draw text centered with shadow
    void drawTextCenteredShadow(const std::string& text, float cx, float y,
                                SF3::Color color = SF3::Color::White(), float scale = 1.0f);

    // Measure text width
    float measureWidth(const std::string& text, float scale = 1.0f);

    // Access font directly if needed
    SF3::Font* font() { return m_font.get(); }
    SF3::Font* smallFont() { return m_smallFont.get(); }

    // Draw with small font
    void drawSmall(const std::string& text, float x, float y,
                   SF3::Color color = SF3::Color::White());
    void drawSmallCentered(const std::string& text, float cx, float y,
                           SF3::Color color = SF3::Color::White());
    float measureSmallWidth(const std::string& text);

private:
    FontManager() = default;
    std::unique_ptr<SF3::Font> m_font;       // Main font (24px)
    std::unique_ptr<SF3::Font> m_smallFont;  // Small font (14px)
};

} // namespace PVZ
