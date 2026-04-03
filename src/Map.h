//
// Created by Weckest on 2026-01-14.
//

#ifndef INC_8051TUTORIAL_MAP_H
#define INC_8051TUTORIAL_MAP_H

#include <vector>
#include <SDL3/SDL.h>
#include <Component.h>

class Map {
public:
    Map() = default;
    ~Map() = default;

    void load(const char *path, SDL_Texture *ts);
    void draw(const Camera& cam);

    SDL_Texture *tileset = nullptr;
    int width{}, height{}, scale{32};
    std::vector<std::vector<int>> tileData;
    std::vector<std::vector<int>> buildingTileData;
    std::vector<std::vector<int>> overlayTileData;
    std::vector<Collider> colliders;
    std::vector<Collider> items;
    std::vector<Transform> spawners;
};

#endif //INC_8051TUTORIAL_MAP_H