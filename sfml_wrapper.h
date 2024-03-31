/******************************************************************************
 * @file
 * @brief
 *
 * @copyright (C) Victor Baldin, 2024.
 *****************************************************************************/

#ifndef SFML_WRAPPER_H_
#define SFML_WRAPPER_H_

#include <assert.h>

#include <SFML/Graphics.hpp>
#include <fontconfig/fontconfig.h>

struct SfmlGui {
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Text text;
    sf::Font font;

    uint8_t* colors;

    unsigned width;
    unsigned height;

    ~SfmlGui() {};
};

struct RgbaPixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct SfmlGuiText {
    const char* font_name;
    sf::Color color;
    unsigned font_size;
};

inline bool sfml_gui_set_font(SfmlGui* gui, const char* name)
{
    bool is_set = true;

    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcNameParse((const FcChar8*)name);
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult res;
    FcPattern* font = FcFontMatch(config, pat, &res);
    if (font != nullptr) {
        FcChar8* file = nullptr;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            if (!gui->font.loadFromFile((char*)file)) {
                is_set = false;
                goto cleanup;
            }
        }
    }

    gui->text.setFont(gui->font);

cleanup:
    FcConfigDestroy(config);
    FcPatternDestroy(pat);
    FcPatternDestroy(font);
    return is_set;
}

inline bool sfml_gui_create(SfmlGui* gui, const char* title,
                            unsigned width, unsigned height,
                            SfmlGuiText text)
{
    assert(gui != nullptr);

    gui->window.create(sf::VideoMode(width, height), title);
    gui->window.setFramerateLimit(60);

    gui->texture.create(width, height);
    gui->sprite.setTexture(gui->texture);

    gui->colors = (uint8_t*)calloc(width * height * 4, sizeof(*gui->colors));
    if (gui->colors == nullptr) {
        fprintf(stderr, "ERROR: Could not create GUI: allocation failure\n");
        return false;
    }

    gui->width = width;
    gui->height = height;
    gui->text.setCharacterSize(text.font_size);
    gui->text.setFillColor(text.color);

    if (!sfml_gui_set_font(gui, text.font_name))
        return false;

    return true;
}

inline void sfml_gui_destroy(SfmlGui* plot)
{
    free(plot->colors);
}


#endif // SFML_WRAPPER_H_
