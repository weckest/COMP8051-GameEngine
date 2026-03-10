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
            // std::cout << entity << std::endl;
            if (entity->hasComponent<Collider>() && entity->hasComponent<Transform>()) {
                auto& t = entity->getComponent<Transform>();

                int xIndex, yIndex;

                getGridIndex(&t.position, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);

                //checks if the entity is outside the world grid then uses the old position as the cell position to remove from
                if (!(xIndex < grid[0].size() && xIndex >= 0) || !(yIndex < grid.size() && yIndex >= 0)) {
                    //outside world. use the old position
                    getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);
                    // std::cout << "Old Grid Position: " << xIndex << "," << yIndex << std::endl;
                }

                //remove the entity
                removeEntity(entity, xIndex, yIndex);

            }
        }
    );

    //print the grid position of the entity
    world.getEventManager().subscribe(
        [this, &world](const BaseEvent& e) {
            if (e.type != EventType::GridDebug) return;
            const auto& debug = static_cast<const GridDebugEvent&>(e);

            auto& map = world.getMap();
            int scale = map.scale;
            int width = map.width * scale;
            int height = map.height * scale;
            auto& grid = world.getEntityGrid();

            int xIndex, yIndex;
            getGridIndex(&debug.entity->getComponent<Transform>().position, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);
            std::cout << "grid Debug: " << xIndex << ", " << yIndex << std::endl;

    });
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
            if ((xIndex < grid[0].size() && xIndex >= 0) && (yIndex < grid.size() && yIndex >= 0)) {

                //check if the old position is out of the grid positions
                //insert into the grid if we know this to be true
                if ((oldXIndex >= grid[0].size() || oldXIndex < 0) || (oldYIndex >= grid.size() || oldYIndex < 0)) {
                    insertEntity(&*e, xIndex, yIndex);
                    continue;
                }

                //the entity has moved cells since the last update
                if (oldXIndex != xIndex || oldYIndex != yIndex) {
                    moveEntity(&*e, oldXIndex, oldYIndex, xIndex, yIndex);
                } else {
                    //add to the list of entities in the cell if it doesnt already exist
                    insertEntity(&*e, xIndex, yIndex);
                }
            }
        }
    }
}

bool GridSystem::moveEntity(Entity *entity, int oldX, int oldY, int newX, int newY) {
    bool insert = insertEntity(entity, newX, newY);
    bool remove = removeEntity(entity, oldX, oldY);
    return insert && remove;
}

bool GridSystem::removeEntity(Entity *entity, int x, int y) {
    auto& grid = world.getEntityGrid();
    //make sure position is in the grid
    if (x >= grid[0].size() || x < 0 || y >= grid.size() || y < 0) return false;

    auto& cell = grid[y][x];

    auto it = std::find(cell.begin(), cell.end(), entity);

    if (it != cell.end()) {
        cell.erase(it);
    } else {
        return false;
    }

    return true;
}

bool GridSystem::insertEntity(Entity *entity, int x, int y) {
    auto& grid = world.getEntityGrid();
    //make sure position is in the grid
    if (x >= grid[0].size() || x < 0 || y >= grid.size() || y < 0) return false;

    auto& cell = grid[y][x];

    auto it = std::find(cell.begin(), cell.end(), entity);

    if (it == cell.end()) {
        cell.push_back(entity);
    } else {
        return false;
    }

    return true;
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

void GridSystem::getGridIndex(
    Vector2D *position, int worldWidth, int worldHeight, int gridX, int gridY, int *xIndex, int *yIndex) {
    //height of the rows as a float
    float rowSize = worldHeight / (gridY * 1.0f);

    //widths of the columns as a float
    float columnSize = worldWidth / (gridX * 1.0f);

    *xIndex = position->x / columnSize;
    *yIndex = position->y / rowSize;
}


void GridSystem::countGridSize() {
    int sum = 0;
    auto& grid = world.getEntityGrid();
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            sum += grid[i][j].size();
        }
    }

    if (sum > world.getEntities().size()) {
        std::cout << "SHITS FUCKED" << std::endl;
    }
}


// void GridSystem::destroyDeferred() {
//     for (int i = 0; i < deferredRemove.size(); i++) {
//         for (int j = 0; j < deferredRemove[0].size(); j++) {
//
//         }
//     }
// }
