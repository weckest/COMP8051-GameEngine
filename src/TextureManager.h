//
// Created by Weckest on 2026-01-14.
//

#ifndef INC_8051TUTORIAL_TEXTUREMANAGER_H
#define INC_8051TUTORIAL_TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

#include "SDL3_Image/SDL_image.h"

class TextureManager {
public:
    static SDL_Texture* load(const char* path);

    //floating point Rect = FRect
    static void draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst);

    static void clean();

private:
    static std::unordered_map<std::string, SDL_Texture*> textures;


};

#endif //INC_8051TUTORIAL_TEXTUREMANAGER_H