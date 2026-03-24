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
#include "data/GridPosition.h"

class World;

class GridSystem {
    World& world;
    SDL_Texture* tex;
    std::unordered_map<int, std::unordered_map<int, bool>> deferredRemove;
public:
    GridSystem(World& world);
    void update(
        std::vector<std::vector<std::vector<Entity*>>> &grid,
        std::vector<std::unique_ptr<Entity>> &entities,
        World &world
    );

    void draw(const Camera& cam);

    static void getGridIndex(Vector2D* position, int worldWidth, int worldHeight, int gridX, int gridY, int* xIndex, int* yIndex);
    static void getGridIndex(Vector2D* position, int worldWidth, int worldHeight, int gridX, int gridY, Vector2D* index);
    static void getGridIndex(Entity* entity, int worldWidth, int worldHeight, int gridX, int gridY, GridPosition* index);

    void createDebugLabels(World &world, int *rows, int *cols);
    void updateCellLabels(World& world);

private:
    // void destroyDeferred();
    void countGridSize();
    bool moveEntity(Entity* entity, int oldX, int oldY, int newX, int newY);
    bool removeEntity(Entity* entity, GridPosition* gridPosition);
    bool removeEntity(Entity* entity, int x, int y);
    bool insertEntity(Entity* entity, GridPosition* gridPosition);
    bool insertEntity(Entity* entity, int x, int y);
};

#endif //INC_8051TUTORIAL_GRIDSYSTEM_H