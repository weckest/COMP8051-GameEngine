//
// Created by Weckest on 2026-03-06.
//

#include "GridSystem.h"
#include "World.h"

GridSystem::GridSystem(World &world) : world(world) {
    tex = TextureManager::load("../assets/colors.png");
    world.getEventManager().subscribe(
        [this, &world](const BaseEvent& e) {
            if (e.type != EventType::Death) return;

            const auto& death = static_cast<const DeathEvent&>(e);
            auto& map = world.getMap();
            int scale = map.scale;
            int width = map.width * scale;
            int height = map.height * scale;
            auto& grid = world.getEntityGrid();

            //remove entity from the grid system
            auto& entity = death.entity;
            if (entity->hasComponent<Collider>() && entity->hasComponent<Transform>()) {
                auto& t = entity->getComponent<Transform>();
                int xIndex, yIndex;

                getGridIndex(&t.position, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);
                auto& cell = grid[yIndex][xIndex];
                std::cout << "before death: " << cell.size() << std::endl;
                // deferredRemove[yIndex][xIndex] = true;
                auto it = std::find(cell.begin(), cell.end(), entity);
                if (it != cell.end()) {
                    *it = cell.back();
                    cell.pop_back();
                }
                std::cout << "after death: " << cell.size() << std::endl;

                //temp debug logging
                if (cell.size() > 150) {
                    std::cout << xIndex << ", " << yIndex << std::endl;
                    for (auto& eT : cell) {
                        std::cout << eT->getComponent<Collider>().tag << std::endl;
                    }
                }
            }

            //TEMP MOVE TO ANOTHER SUBSCRIBER
            entity->destroy();
        }
    );
}

void GridSystem::update(
    std::vector<std::vector<std::vector<Entity*>>>& grid,
    std::vector<std::unique_ptr<Entity>>& entities,
    World &world
) {
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    for (auto& e: entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            auto& t = e->getComponent<Transform>();

            int oldXIndex, oldYIndex;
            int xIndex, yIndex;

            getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &oldXIndex, &oldYIndex);
            getGridIndex(&t.position, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);

            //only do this if the entity is with in the world bounds
            if ((xIndex < grid[0].size() && xIndex > 0) && (yIndex < grid.size() && yIndex > 0)) {
                auto& newCell = grid[yIndex][xIndex];
                auto& oldCell = grid[oldYIndex][oldXIndex];


                //the entity has moved cells since the last update
                if (oldXIndex != xIndex || oldYIndex != yIndex) {
                    std::cout << "entity " << e.get() << " is going to new cell, X: " << xIndex << ", Y:" << yIndex << std::endl;
                    newCell.push_back(e.get());
                    auto it = std::find(oldCell.begin(), oldCell.end(), e.get());
                    if (it != oldCell.end()) {
                        *it = oldCell.back();
                        oldCell.pop_back();
                    }
                    std::cout << "New X: " << xIndex << ", Y: " << yIndex << std::endl;
                    std::cout << "Old X: " << oldXIndex << ", Y: " << oldYIndex << std::endl;
                    std::cout << "New Size: " << newCell.size() << ", Old Size: " << oldCell.size() << std::endl;

                } else {
                    //add to the list of entities in the cell if it doesnt already exist
                    if (std::find(newCell.begin(), newCell.end(), e.get()) == newCell.end()) {
                        newCell.push_back(e.get());
                    }
                }
            }
        }
    }
}

void GridSystem::draw(const Camera &cam) {
    SDL_FRect src{96, 0, 32, 32}, dest{};
    int stripeWidth = 8;
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    auto& grid = world.getEntityGrid();
    int colWidth = width / grid[0].size();
    int rowHeight = height / grid.size();

    for (int y = 0; y < grid.size(); y++) {
        float worldY = static_cast<float>(y) * rowHeight * 1.0f;

        dest.y = std::round(worldY - cam.view.y) - stripeWidth / 2;
        dest.x = 0;
        dest.h = stripeWidth;
        dest.w = width;

        TextureManager::draw(tex, src, dest);
    }

    for (int x = 0; x < grid[0].size(); x++) {
        float worldX = static_cast<float>(x) * colWidth * 1.0f;

        //move the tiles or map relative to the camera
        //convert from world space to screen space
        dest.x = std::round(worldX - cam.view.x) - stripeWidth / 2;
        dest.y = 0;
        dest.w = stripeWidth;
        dest.h = height;

        TextureManager::draw(tex, src, dest);
    }
}

// void GridSystem::destroyDeferred() {
//     for (int i = 0; i < deferredRemove.size(); i++) {
//         for (int j = 0; j < deferredRemove[0].size(); j++) {
//
//         }
//     }
// }
