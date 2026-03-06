//
// Created by Weckest on 2026-03-06.
//

#ifndef INC_8051TUTORIAL_GRIDSYSTEM_H
#define INC_8051TUTORIAL_GRIDSYSTEM_H
#include <iostream>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World;

class GridSystem {
    World& world;
    SDL_Texture* tex;
public:
    GridSystem(World& world);
    void update(
        std::vector<std::vector<std::vector<Entity*>>>& grid,
        std::vector<std::unique_ptr<Entity>>& entities,
        World& world
    );

    void draw(const Camera& cam);

    static void getGridIndex(Vector2D* position, int worldWidth, int worldHeight, int gridX, int gridY, int* xIndex, int* yIndex) {
        //height of the rows as a float
        float rowSize = worldHeight / (gridY * 1.0f);

        //widths of the columns as a float
        float columnSize = worldWidth / (gridX * 1.0f);

        *xIndex = position->x / columnSize;
        *yIndex = position->y / rowSize;
    }
};

#endif //INC_8051TUTORIAL_GRIDSYSTEM_H