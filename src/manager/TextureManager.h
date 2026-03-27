//
// Created by Weckest on 2026-01-14.
//

#ifndef INC_8051TUTORIAL_TEXTUREMANAGER_H
#define INC_8051TUTORIAL_TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

#include "Vector2D.h"
#include "SDL3_Image/SDL_image.h"

#include "Component.h"

class TextureManager {
public:
    static SDL_Texture* load(const char* path);

    static void loadLabel(Label& label);
    static void updateLabel(Label& label);

    //floating point Rect = FRect
    static void draw(SDL_Texture* texture, const SDL_FRect* src, const SDL_FRect* dst);

    //rotation overload
    static void draw(SDL_Texture* texture, const SDL_FRect* src, const SDL_FRect* dst, float rotation, SDL_FPoint* center);

    static void drawLine(Vector2D start, Vector2D end);

    static void drawLine(Vector2D start, Vector2D end, Uint8 r, Uint8 g, Uint8 b);

    static void drawCircle(Vector2D& center, float radius, Uint8 r, Uint8 g, Uint8 b);

    static void drawRect(Vector2D& tl, Vector2D& br, Uint8 r, Uint8 g, Uint8 b);

    static void clean();

private:
    static std::unordered_map<std::string, SDL_Texture*> textures;


};

#endif //INC_8051TUTORIAL_TEXTUREMANAGER_H