//
// Created by Weckest on 2026-02-04.
//

#include "CollisionSystem.h"

#include <iostream>
#include <unordered_set>

#include "Collision.h"
#include "World.h"

CollisionSystem::CollisionSystem(World &world) {
    world.getEventManager().subscribe(
        [this](const BaseEvent& e) {
            if (e.type != EventType::Death) return;

            const auto& death = static_cast<const DeathEvent&>(e);
            // std::cout << "Death " << &*death.entity << std::endl;
            std::vector<std::pair<Entity*, Entity*>> remove;
            // std::cout << "looking for: " << death.entity << std::endl;
            for (std::pair<Entity*, Entity*> key: activeCollisions) {
                // std::cout << " " << key.first << ", " << key.second << std::endl;
                if (key.first == death.entity || key.second == death.entity) {
                    remove.push_back(key);
                }
            }
            // std::cout << remove.size() << std::endl;
            for (std::pair<Entity*, Entity*> key: remove) {
                activeCollisions.erase(key);
            }
        });
}

void CollisionSystem::update(World &world, Timer& timer) {

    //get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());
    const std::vector<std::vector<std::vector<Entity*>>>& grid = world.getEntityGrid();

    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;

    std::unordered_set<CollisionKey, CollisionKeyHash> currentCollisions{};
    std::vector<CollisionEvent> events;

    //update all collider positions first
    for (auto entity: collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;

        //have to resize due to how the spritesheet is set up
        if (entity->hasComponent<PlayerTag>() || entity->hasComponent<EnemyTag>()) {

            auto& spr = entity->getComponent<Sprite>();

            c.rect.x += spr.dst.w / 4;
            c.rect.y += spr.dst.h / 4 * 2;
        }

    }

    //outer loop
    timer.startTimer("outerLoop");
    for (size_t i = 0; i < collidables.size(); i++) {

        auto entityA = collidables[i];

        // std::cout << "Entity A " << entityA << std::endl;

        //dont do collisions if the entity is dead
        if (!entityA->isActive()) continue;

        auto& colliderA = entityA->getComponent<Collider>();
        auto& transformA = entityA->getComponent<Transform>();

        GridPosition gridPosition{};
        GridSystem::getGridIndex(entityA, width, height, grid[0].size(), grid.size(), &gridPosition);

        for (int xIndex = gridPosition.tl.x; xIndex <= gridPosition.br.x; xIndex++) {
            for (int yIndex = gridPosition.tl.y; yIndex <= gridPosition.br.y; yIndex++) {
                if ((xIndex < grid[0].size() && xIndex >= 0) && (yIndex < grid.size() && yIndex >= 0)) {
                    auto& cell = grid[yIndex][xIndex];

                    //check for the collider collision
                    //inner loop
                    for (auto& entityB : cell) {
                        auto& colliderB = entityB->getComponent<Collider>();

                        if (entityA == entityB) continue;
                        //dont do collisions if the entity is dead
                        if (!entityB->isActive()) continue;
                        // if (!entityB->hasComponent<Collider>()) continue;

                        if (colliderA.mask & colliderB.layer) {
                            if (Collision::AABB(colliderA, colliderB)) {
                                CollisionKey key = makeKey(entityA, entityB);
                                if (currentCollisions.contains(key)) continue;
                                currentCollisions.insert(key);

                                if (!activeCollisions.contains(key)) {
                                    events.emplace_back(entityA, entityB, CollisionState::Enter);
                                } else {
                                    events.emplace_back(entityA, entityB, CollisionState::Stay);
                                }
                            }
                            break;
                        }


                    }
                }
            }
        }
    }
    timer.stopTimer("outerLoop");

    EventManager em = world.getEventManager();
    for (CollisionEvent event: events) {
        em.emit(event);
    }

    timer.startTimer("activeCollisions");
    for (auto& key: activeCollisions) {
        if (!currentCollisions.contains(key)) {
            if (key.first->isActive() == 1 && key.second->isActive() == 1) {
                world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
            }
        }
    }
    timer.stopTimer("activeCollisions");

    activeCollisions = std::move(currentCollisions); //update with current collisions
}

std::vector<Entity*> CollisionSystem::getAllWithin(World &world, Entity &entity, float distance) {
    std::vector<Entity*> collidables;

    // Require at least Transform
    if (!entity.hasComponent<Transform>()) return collidables;

    auto& et = entity.getComponent<Transform>();
    Vector2D eTemp = et.position;

    if (entity.hasComponent<Sprite>()) {
        auto& es = entity.getComponent<Sprite>();
        eTemp.x += es.dst.w;
        eTemp.y += es.dst.h;
    }

    for (auto& ePtr : world.getEntities()) {
        if (!ePtr) continue; // skip null entity pointers
        Entity* e = ePtr.get();

        if (!e->hasComponent<Transform>() || !e->hasComponent<EnemyTag>()) continue;

        Vector2D temp = e->getComponent<Transform>().position;
        if (e->hasComponent<Sprite>()) {
            auto& s = e->getComponent<Sprite>();
            temp.x += s.dst.w;
            temp.y += s.dst.h;
        }

        if ((temp - eTemp).length() <= distance) {
            collidables.push_back(e);
        }
    }

    return collidables;
}

Entity * CollisionSystem::getClosestEntity(World& world, Entity &entity, float radius) {
    if (entity.hasComponent<Transform>() && (entity.hasComponent<Sprite>() || entity.hasComponent<Collider>())) {
        auto& grid = world.getEntityGrid();
        auto& map = world.getMap();
        int scale = map.scale;
        int width = map.width * scale;
        int height = map.height * scale;
        auto& t = entity.getComponent<Transform>();
        if (!&t) return nullptr;
        Vector2D centerPoint = t.position;
        Vector2D topLeft{};
        Vector2D bottomRight{};

        if (entity.hasComponent<Sprite>()) {
            auto& s = entity.getComponent<Sprite>();
            centerPoint.x += s.dst.w / 2;
            centerPoint.y += s.dst.h / 2;
        } else {
            auto& c = entity.getComponent<Collider>();
            centerPoint.x += c.rect.w / 2;
            centerPoint.y += c.rect.h / 2;
        }

        //move the top left to include the distance
        topLeft.x = centerPoint.x - radius;
        topLeft.y = centerPoint.y - radius;

        //move the bottom right to include the distance
        bottomRight.x = centerPoint.x + radius;
        bottomRight.y = centerPoint.y + radius;

        GridPosition gridPosition{};
        GridSystem::getGridIndex(&topLeft, width, height, grid[0].size(), grid.size(), &gridPosition.tl);
        GridSystem::getGridIndex(&bottomRight, width, height, grid[0].size(), grid.size(), &gridPosition.br);

        float distance = 0;
        Entity* closestEntity = nullptr;
        for (int xIndex = gridPosition.tl.x; xIndex <= gridPosition.br.x; xIndex++) {
            for (int yIndex = gridPosition.tl.y; yIndex <= gridPosition.br.y; yIndex++) {
                if (xIndex < grid[0].size() && xIndex >= 0 && yIndex < grid.size() && yIndex >= 0) {
                    auto& cell = grid[yIndex][xIndex];

                    // std::cout << "Checking: " <<  xIndex << " " << yIndex << std::endl;

                    for (auto& entityB : cell) {
                        if (entityB->hasComponent<Transform>() &&
                            (entityB->hasComponent<Sprite>() || entityB->hasComponent<Collider>()) &&
                            entityB->hasComponent<EnemyTag>()
                        ) {
                            // std::cout << entityB << std::endl;
                            auto& bT = entityB->getComponent<Transform>();
                            Vector2D bCenterPoint = bT.position;

                            if (entityB->hasComponent<Sprite>()) {
                                auto& s = entityB->getComponent<Sprite>();
                                bCenterPoint.x += s.dst.w / 2;
                                bCenterPoint.y += s.dst.h / 2;
                            } else {
                                auto& c = entityB->getComponent<Collider>();
                                bCenterPoint.x -= c.rect.w / 2;
                                bCenterPoint.y -= c.rect.h / 2;
                            }

                            if ((bCenterPoint - centerPoint).length() > radius) continue;

                            if (closestEntity == nullptr) {
                                closestEntity = entityB;
                                distance = (centerPoint - bCenterPoint).length();
                            } else if ((centerPoint - bCenterPoint).length() < distance) {
                                closestEntity = entityB;
                                distance = (centerPoint - bCenterPoint).length();
                            }
                        }
                    }

                }
            }
        }
        return closestEntity;
    }

    return nullptr;
}


std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities) {
    std::vector<Entity*> collidables;
    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }
    return collidables;
}
