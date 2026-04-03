//
// Created by migue on 2026-04-01.
//

#ifndef INC_8051TUTORIAL_OVERLAYRENDERSYSTEM_H
#define INC_8051TUTORIAL_OVERLAYRENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class OverlayRenderSystem {
public:
    SDL_Texture *tileset = nullptr;
    bool hasData = false;
    int width{}, height{}, scale{32};
    void receiveData(std::vector<std::vector<int>> data, int width, int height, int scale, SDL_Texture *tileset);
    void draw(const Camera& cam);
private:
    std::vector<std::vector<int>> overlayData;
};


#endif //INC_8051TUTORIAL_OVERLAYRENDERSYSTEM_H