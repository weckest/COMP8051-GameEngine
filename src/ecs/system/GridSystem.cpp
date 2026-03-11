//
// Created by Weckest on 2026-03-06.
//

#include "GridSystem.h"
#include "World.h"
#include "utils/data/GridPosition.h"

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

                GridPosition gridPosition{};

                getGridIndex(entity, width, height, grid[0].size(), grid.size(), &gridPosition);

                // std::cout << "Before any checks" << std::endl;
                // std::cout << entity << ", TLX: " << gridPosition.tl.x << ", TLY: " << gridPosition.tl.y << ", BRX: " << gridPosition.br.x << ", BRY: " << gridPosition.br.y << std::endl;


                //check if the top left is outside the grid
                if (!(gridPosition.tl.x < grid[0].size() && gridPosition.tl.x >= 0) || !(gridPosition.tl.y < grid.size() && gridPosition.tl.y >= 0)) {
                    //outside world. use the old position
                    // getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &xIndex, &yIndex);
                    // getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &gridPosition.br);
                    getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &gridPosition.tl);

                    // std::cout << "Old Grid Position: " << xIndex << "," << yIndex << std::endl;
                }

                // std::cout << "Before BR Checking" << std::endl;
                // std::cout << entity << ", TLX: " << gridPosition.tl.x << ", TLY: " << gridPosition.tl.y << ", BRX: " << gridPosition.br.x << ", BRY: " << gridPosition.br.y << std::endl;


                //check if the bottom right is outside the grid
                if (!(gridPosition.br.x < grid[0].size() && gridPosition.br.x >= 0) || !(gridPosition.br.y < grid.size() && gridPosition.br.y >= 0)) {

                    Vector2D oldPosition = t.oldPosition;
                    if (entity->hasComponent<Sprite>()) {
                        auto& s = entity->getComponent<Sprite>();
                        oldPosition.x += s.dst.w;
                        oldPosition.y += s.dst.h;
                    } else if (entity->hasComponent<Collider>()) {
                        auto& c = entity->getComponent<Collider>();
                        oldPosition.x += c.rect.w;
                        oldPosition.y += c.rect.h;
                    }

                    // std::cout << "using the old position" << std::endl;

                    getGridIndex(&oldPosition, width, height, grid[0].size(), grid.size(), &gridPosition.br);
                }

                // std::cout << entity << ", TLX: " << gridPosition.tl.x << ", TLY: " << gridPosition.tl.y << ", BRX: " << gridPosition.br.x << ", BRY: " << gridPosition.br.y << std::endl;

                //remove the entity from all cells
                for (int xIndex = (int)gridPosition.tl.x; xIndex <= gridPosition.br.x; xIndex++) {
                    for (int yIndex = (int)gridPosition.tl.y; yIndex <= gridPosition.br.y; yIndex++) {
                        if (xIndex >= grid[0].size() || yIndex >= grid.size() || xIndex < 0 || yIndex < 0) {
                            continue;
                        }
                        removeEntity(entity, xIndex, yIndex);
                    }
                }

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
            auto& c = e->getComponent<Collider>();

            Vector2D entityWH{t.position.x + c.rect.w, t.position.y + c.rect.h};
            Vector2D oldEntityWH{t.oldPosition.x + c.rect.w, t.oldPosition.y + c.rect.h};

            if (e->hasComponent<Sprite>()) {
                auto& s = e->getComponent<Sprite>();
                entityWH.x = t.position.x + s.dst.w;
                entityWH.y = t.position.y + s.dst.h;
                oldEntityWH.x = t.oldPosition.x + s.dst.w;
                oldEntityWH.y = t.oldPosition.y + s.dst.h;
            }

            GridPosition gridPosition{};
            GridPosition oldGridPosition{};

            //want to get the grid position of the transform and then also the bottom right of the sprite
            //then loop over each position and insert the entity into the cell
            //this also has to be done for the old position
            //then loop over each cell and remove the ones that arent overlapping

            getGridIndex(&entityWH, width, height, grid[0].size(), grid.size(), &gridPosition.br);
            getGridIndex(&oldEntityWH, width, height, grid[0].size(), grid.size(), &oldGridPosition.br);
            getGridIndex(&t.position, width, height, grid[0].size(), grid.size(), &gridPosition.tl);
            getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &oldGridPosition.tl);

            //loop over each of the old position cells and remove the entity from that cell
            for (int oldXIndex = oldGridPosition.tl.x; oldXIndex <= oldGridPosition.br.x; oldXIndex++) {
                for (int oldYIndex = oldGridPosition.tl.y; oldYIndex <= oldGridPosition.br.y; oldYIndex++) {
                    if ((oldXIndex < grid[0].size() && oldXIndex >= 0) && (oldYIndex < grid.size() && oldYIndex >= 0)) {
                        removeEntity(&*e, oldXIndex, oldYIndex);
                    }
                }
            }

            //loop over each position and insert into that cell
            for (int xIndex = gridPosition.tl.x; xIndex <= gridPosition.br.x; xIndex++) {
                for (int yIndex = gridPosition.tl.y; yIndex <= gridPosition.br.y; yIndex++) {
                    //only do this if the entity is with in the world bounds
                    if ((xIndex < grid[0].size() && xIndex >= 0) && (yIndex < grid.size() && yIndex >= 0)) {
                        //add to the list of entities in the cell if it doesnt already exist
                        insertEntity(&*e, xIndex, yIndex);
                    }
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

    *xIndex = (int)(position->x / columnSize);
    *yIndex = (int)(position->y / rowSize);
}

void GridSystem::getGridIndex(Vector2D *position, int worldWidth, int worldHeight, int gridX, int gridY,
    Vector2D *index) {
    //height of the rows as a float
    float rowSize = worldHeight / (gridY * 1.0f);

    //widths of the columns as a float
    float columnSize = worldWidth / (gridX * 1.0f);

    index->x = (int)(position->x / columnSize);
    index->y = (int)(position->y / rowSize);
}

void GridSystem::getGridIndex(Entity *entity, int worldWidth, int worldHeight, int gridX, int gridY,
    GridPosition *index) {
    if (entity->hasComponent<Transform>() && (entity->hasComponent<Collider>() || entity->hasComponent<Sprite>())) {
        auto& t = entity->getComponent<Transform>();
        Vector2D entityWH = t.position;

        if (entity->hasComponent<Sprite>()) {
            auto& s = entity->getComponent<Sprite>();
            entityWH.x += s.dst.w;
            entityWH.y += s.dst.h;
        } else {
            auto& c = entity->getComponent<Collider>();
            entityWH.x += c.rect.w;
            entityWH.y += c.rect.h;
        }

        getGridIndex(&t.position, worldWidth, worldHeight, gridX, gridY, &index->tl);
        getGridIndex(&entityWH, worldWidth, worldHeight, gridX, gridY, &index->br);
    }
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
