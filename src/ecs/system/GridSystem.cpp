//
// Created by Weckest on 2026-03-06.
//

#include "GridSystem.h"
#include "World.h"
#include "manager/AssetManager.h"
#include "utils/data/GridPosition.h"

GridSystem::GridSystem(World &world) : world(world) {
    tex = TextureManager::load("assets/colors.png");
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

                Vector2D br = t.position;

                if (entity->hasComponent<Sprite>()) {
                        auto& s = entity->getComponent<Sprite>();
                        br.x += s.dst.w;
                        br.y += s.dst.h;
                    } else if (entity->hasComponent<Collider>()) {
                        auto& c = entity->getComponent<Collider>();
                        br.x += c.rect.w;
                        br.y += c.rect.h;
                    }

                // std::cout << "Death: " << entity << " " << t.position << std::endl;
                GridPosition gridPosition{};

                getGridIndex(&t.position, width, height, grid[0].size(), grid.size(), &gridPosition.tl);
                getGridIndex(&br, width, height, grid[0].size(), grid.size(), &gridPosition.br);

                // std::cout << gridPosition.tl << " " << gridPosition.br << std::endl;

                //check if the top left is outside the grid
                if (!(gridPosition.tl.x < grid[0].size() && gridPosition.tl.x >= 0) || !(gridPosition.tl.y < grid.size() && gridPosition.tl.y >= 0)) {
                    if (entity->hasComponent<RingFireTag>()) {
                        gridPosition.tl.x = std::max(gridPosition.tl.x, 0.0f);
                        gridPosition.tl.y = std::max(gridPosition.tl.y, 0.0f);
                    } else {
                        //outside world. use the old position
                        // std::cout << "top left outside the grid" << std::endl;
                        getGridIndex(&t.oldPosition, width, height, grid[0].size(), grid.size(), &gridPosition.tl);
                    }

                }

                //check if the bottom right is outside the grid
                if (!(gridPosition.br.x < grid[0].size() && gridPosition.br.x >= 0) || !(gridPosition.br.y < grid.size() && gridPosition.br.y >= 0)) {
                    // std::cout << "bottom right outside the grid" << std::endl;

                    if (entity->hasComponent<RingFireTag>()) {
                        gridPosition.br.x = std::min(grid[0].size() * 1.0f, gridPosition.br.x);
                        gridPosition.br.y = std::min(grid.size() * 1.0f, gridPosition.br.y);
                    } else {
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

                        getGridIndex(&oldPosition, width, height, grid[0].size(), grid.size(), &gridPosition.br);
                    }
                }

                //remove the entity from all cells
                // std::cout << "Removing " << entity << " from: " << gridPosition.tl << " to " << gridPosition.br << std::endl;
                for (int xIndex = (int)gridPosition.tl.x; xIndex <= gridPosition.br.x; xIndex++) {
                    for (int yIndex = (int)gridPosition.tl.y; yIndex <= gridPosition.br.y; yIndex++) {
                        if (xIndex >= grid[0].size() || yIndex >= grid.size() || xIndex < 0 || yIndex < 0) {
                            continue;
                        }
                        // std::cout << "Removing: " << entity  << " (" << xIndex << "," << yIndex << ")" << std::endl;
                        removeEntity(entity, xIndex, yIndex);
                    }
                }
            }
        }
    );

    world.getEventManager().subscribe(
        [this, &world](const BaseEvent& event) {
            if (event.type != EventType::Move) return;

            const auto& move = static_cast<const MoveEvent&>(event);
            auto& e = move.entity;

            auto& map = world.getMap();
            int scale = map.scale;
            int width = map.width * scale;
            int height = map.height * scale;

            auto& grid = world.getEntityGrid();

            if (e->hasComponent<Transform>() && e->hasComponent<Collider>() && e->isActive() && !e->hasComponent<Clickable>()) {
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

                // std::cout << "New: " << t.position.x << " " << t.position.y << " " << entityWH << std::endl;
                // std::cout << "Old: " << t.oldPosition.x << " " << t.oldPosition.y << " " << oldEntityWH << std::endl;

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

                if (gridPosition.tl != oldGridPosition.tl || gridPosition.br != oldGridPosition.br) {
                    //remove the entity from the cells
                    removeEntity(&*e, &oldGridPosition);
                }

                //inserts the entity into the cells
                insertEntity(&*e, &gridPosition);

                float rangeY = 255.0f / grid.size();
                float rangeX = 255.0f / grid[0].size();
                // std::cout << rangeX << " " << rangeY << std::endl;
                c.r = rangeX * gridPosition.tl.x;
                c.g = rangeY * gridPosition.tl.y;
                // c.b = range * gridPosition.br.x;

            }
        }
    );
}

void GridSystem::update(
    std::vector<std::vector<std::vector<Entity*>>>& grid,
    std::vector<std::unique_ptr<Entity>>& entities,
    World &world
) {
    // std::cout << "Updating Grid " << entities.size() << std::endl;
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    for (auto& e: entities) {
        // std::cout << "Grid: " << e << std::endl;
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>() && e->isActive() && !e->hasComponent<Clickable>()) {
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

            // std::cout << "New: " << t.position.x << " " << t.position.y << " " << entityWH << std::endl;
            // std::cout << "Old: " << t.oldPosition.x << " " << t.oldPosition.y << " " << oldEntityWH << std::endl;

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

            if (gridPosition.tl != oldGridPosition.tl || gridPosition.br != oldGridPosition.br) {
                //remove the entity from the cells
                removeEntity(&*e, &oldGridPosition);
            }

                //inserts the entity into the cells
                insertEntity(&*e, &gridPosition);

                float rangeY = 255.0f / grid.size();
                float rangeX = 255.0f / grid[0].size();
                // std::cout << rangeX << " " << rangeY << std::endl;
                c.r = rangeX * gridPosition.tl.x;
                c.g = rangeY * gridPosition.tl.y;
                // c.b = range * gridPosition.br.x;

        }
    }
}

bool GridSystem::moveEntity(Entity *entity, int oldX, int oldY, int newX, int newY) {
    bool insert = insertEntity(entity, newX, newY);
    bool remove = removeEntity(entity, oldX, oldY);
    return insert && remove;
}

bool GridSystem::removeEntity(Entity *entity, GridPosition *gridPosition) {
    auto& grid = world.getEntityGrid();
    // std::cout << "Removing " << entity << " from: " << gridPosition->tl << " to " << gridPosition->br << std::endl;
    for (int oldXIndex = gridPosition->tl.x; oldXIndex <= gridPosition->br.x; oldXIndex++) {
        for (int oldYIndex = gridPosition->tl.y; oldYIndex <= gridPosition->br.y; oldYIndex++) {
            if ((oldXIndex < grid[0].size() && oldXIndex >= 0) && (oldYIndex < grid.size() && oldYIndex >= 0)) {
                if (!removeEntity(&*entity, oldXIndex, oldYIndex)) return false;
            }
        }
    }
    return true;
}

bool GridSystem::removeEntity(Entity *entity, int x, int y) {
    auto& grid = world.getEntityGrid();

    //make sure position is in the grid
    if (x >= grid[0].size() || x < 0 || y >= grid.size() || y < 0) return false;

    auto& cell = grid[y][x];

    auto it = std::find(cell.begin(), cell.end(), entity);

    if (it != cell.end()) {
        // std::cout << "Removing: " << entity  << " (" << x << "," << y << ")" << std::endl;
        cell.erase(it);
    } else {
        return false;
    }

    return true;
}

bool GridSystem::insertEntity(Entity *entity, GridPosition *gridPosition) {
    auto& grid = world.getEntityGrid();
    // std::cout << "Inserting " << entity << " into: " << gridPosition->tl << " " << gridPosition->br << std::endl;
    for (int xIndex = gridPosition->tl.x; xIndex <= gridPosition->br.x; xIndex++) {
        for (int yIndex = gridPosition->tl.y; yIndex <= gridPosition->br.y; yIndex++) {
            //only do this if the entity is with in the world bounds
            if ((xIndex < grid[0].size() && xIndex >= 0) && (yIndex < grid.size() && yIndex >= 0)) {
                //add to the list of entities in the cell if it doesnt already exist
                insertEntity(entity, xIndex, yIndex);
            }
        }
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
        // std::cout << "Inserting: " << entity  << " (" << x << "," << y << ")"  << std::endl;
        cell.push_back(entity);
    } else {
        return false;
    }

    return true;
}

void GridSystem::draw(const Camera &cam) {
    SDL_FRect src{96, 0, 32, 32}, dest{};
    int stripeWidth = 4;
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    auto& grid = world.getEntityGrid();
    float colWidth = width * 1.0f / grid[0].size();
    float rowHeight = height * 1.0f / grid.size();

    for (int y = 0; y <= grid.size(); y++) {
        float worldY = y * rowHeight;

        dest.y = std::round(worldY - cam.view.y) - stripeWidth / 2;
        dest.x = 0;
        dest.h = stripeWidth;
        dest.w = width;

        TextureManager::draw(tex, &src, &dest);
    }

    for (int x = 0; x <= grid[0].size(); x++) {
        float worldX = x * colWidth;

        //move the tiles or map relative to the camera
        //convert from world space to screen space
        dest.x = std::round(worldX - cam.view.x) - stripeWidth / 2;
        dest.y = 0;
        dest.w = stripeWidth;
        dest.h = height;

        TextureManager::draw(tex, &src, &dest);
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
        index->br.x++;
        index->br.y++;

        if (entity->hasComponent<PlayerTag>()) {
            // std::cout << entityWH << "\n" << index->tl << "\n" <<  index->br << "\n" << std::endl;
        }
    }
}

void GridSystem::createDebugLabels(World &world, int* rows, int* cols) {
    Label label = {
        "0",
        AssetManager::getFont("arial"),
        {255,255,255,255},
        LabelType::Debug,
        "gridEntityCount"
    };
    label.visible = false;

    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    float rangeY = height / (*rows * 1.0f);
    float rangeX = width / (*cols * 1.0f);
    // int sum = 0;
    //create debug labels for each cells number of entities
    for (int x = 0; x < *cols; x++) {
        for (int y = 0; y < *rows; y++) {
            // std::cout << rangeX * x << " " << rangeY * y << std::endl;
            //create label at position of cell

            auto& entity = world.createEntity();
            auto& cell = world.getEntityGrid()[y][x];
            label.text = "0";
            TextureManager::loadLabel(label);
            entity.addComponent<Label>(label);
            entity.addComponent<Transform>(Vector2D(rangeX * x + 2.5f, rangeY * y + 2.5f), 0.0f, 1.0f);
            // sum++;
        }
    }
    // std::cout << sum << std::endl;
}

void GridSystem::updateCellLabels(World &world) {
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    Entity* cam = nullptr;

    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<Camera>()) {
            cam = entity.get();
        }
    }

    if (!cam) return;
    auto& camera = cam->getComponent<Camera>();

    auto& entityGrid = world.getEntityGrid();
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<Label>() && entity->getComponent<Label>().type == LabelType::Debug) {
            auto& transform = entity->getComponent<Transform>();
            Vector2D index;

            getGridIndex(&transform.position, width, height, entityGrid[0].size(), entityGrid.size(), &index);

            if (index.x < 0 || index.x >= width || index.y < 0 || index.y >= height) continue;

            if (camera.view.x < transform.position.x && camera.view.x + camera.worldWidth > transform.position.x) {
                if (camera.view.y < transform.position.y && camera.view.y + camera.worldHeight > transform.position.y) {
                    auto& label = entity->getComponent<Label>();
                    if (label.text == std::to_string(entityGrid[index.y][index.x].size())) continue;
                    // std::cout << index << std::endl;
                    label.text = std::to_string(entityGrid[index.y][index.x].size());
                    label.dirty = true;
                }
            }
        }
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
