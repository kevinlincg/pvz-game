#include "pvz_font.hpp"

namespace PVZ {

FontManager& FontManager::instance() {
    static FontManager inst;
    return inst;
}

bool FontManager::init() {
    // Try multiple font paths for cross-platform support
    // Prefer .ttf (single font) over .ttc (collection, may need offset handling)
    const char* fontPaths[] = {
        // Subset font for web build (214KB vs 22MB)
        "assets/fonts/pvz_font_subset.ttf",
        // Bundled font (preferred - cross-platform)
        "assets/fonts/arial_unicode.ttf",
        // macOS
        "/Library/Fonts/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        // Windows
        "C:\\Windows\\Fonts\\msyh.ttf",     // Microsoft YaHei
        "C:\\Windows\\Fonts\\msjh.ttf",     // Microsoft JhengHei (Traditional)
        "C:\\Windows\\Fonts\\ARIALUNI.TTF",  // Arial Unicode MS
        "C:\\Windows\\Fonts\\simsun.ttf",    // SimSun
        // Linux
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
        nullptr
    };

    for (int i = 0; fontPaths[i]; i++) {
        m_font = SF3::Font::load(fontPaths[i], 24);
        if (m_font && m_font->valid()) {
            m_smallFont = SF3::Font::load(fontPaths[i], 14);
            SF3::Log::info("PVZ Font loaded: %s", fontPaths[i]);
            return true;
        }
    }

    SF3::Log::error("PVZ FontManager: No Chinese font found!");
    return false;
}

void FontManager::drawText(const std::string& text, float x, float y,
                           SF3::Color color, float scale) {
    if (!m_font) return;
    // Scale not directly supported by Font::draw, use as-is for now
    (void)scale;
    SF3::Font::draw(*m_font, text, x, y, color);
}

void FontManager::drawTextCentered(const std::string& text, float cx, float y,
                                   SF3::Color color, float scale) {
    if (!m_font) return;
    float w = m_font->measureWidth(text);
    (void)scale;
    SF3::Font::draw(*m_font, text, cx - w * 0.5f, y, color);
}

void FontManager::drawTextRight(const std::string& text, float rightX, float y,
                                SF3::Color color, float scale) {
    if (!m_font) return;
    float w = m_font->measureWidth(text);
    (void)scale;
    SF3::Font::draw(*m_font, text, rightX - w, y, color);
}

void FontManager::drawTextShadow(const std::string& text, float x, float y,
                                 SF3::Color color, float scale) {
    if (!m_font) return;
    (void)scale;
    SF3::Font::draw(*m_font, text, x + 1, y + 1, SF3::Color(0, 0, 0, 180));
    SF3::Font::draw(*m_font, text, x, y, color);
}

void FontManager::drawTextCenteredShadow(const std::string& text, float cx, float y,
                                         SF3::Color color, float scale) {
    if (!m_font) return;
    float w = m_font->measureWidth(text);
    float x = cx - w * 0.5f;
    (void)scale;
    SF3::Font::draw(*m_font, text, x + 1, y + 1, SF3::Color(0, 0, 0, 180));
    SF3::Font::draw(*m_font, text, x, y, color);
}

float FontManager::measureWidth(const std::string& text, float scale) {
    if (!m_font) return 0;
    (void)scale;
    return m_font->measureWidth(text);
}

void FontManager::drawSmall(const std::string& text, float x, float y,
                            SF3::Color color) {
    if (!m_smallFont) return;
    SF3::Font::draw(*m_smallFont, text, x, y, color);
}

void FontManager::drawSmallCentered(const std::string& text, float cx, float y,
                                    SF3::Color color) {
    if (!m_smallFont) return;
    float w = m_smallFont->measureWidth(text);
    SF3::Font::draw(*m_smallFont, text, cx - w * 0.5f, y, color);
}

float FontManager::measureSmallWidth(const std::string& text) {
    if (!m_smallFont) return 0;
    return m_smallFont->measureWidth(text);
}

} // namespace PVZ
