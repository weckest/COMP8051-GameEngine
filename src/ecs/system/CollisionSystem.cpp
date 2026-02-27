//
// Created by Weckest on 2026-02-04.
//

#include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "World.h"

void CollisionSystem::update(World &world) {

    //get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());


    //update all collider positions first
    for (auto entity: collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;
    }

    //outer loop
    for (size_t i = 0; i < collidables.size(); i++) {
        auto entityA = collidables[i];
        auto& colliderA = entityA->getComponent<Collider>();

        //check for the collider collision
        //inner loop
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<Collider>();
            if (Collision::AABB(colliderA, colliderB)) {
                if ((colliderA.tag == "bullet" && colliderB.tag == "enemy") ||
                    (colliderA.tag == "enemy" && colliderB.tag == "bullet")) {
                        std::cout << "Bullet-Enemy AABB detected\n";
                    }
                world.getEventManager().emit(CollisionEvent{entityA, entityB});
            }
        }
    }
}

std::vector<Entity*> CollisionSystem::getAllWithin(World &world, Entity &entity, float distance) {
    std::vector<Entity*> collidables;
    if (!entity.hasComponent<Transform>() && !entity.hasComponent<Sprite>()) return collidables;

    auto& et = entity.getComponent<Transform>();
    auto& es = entity.getComponent<Sprite>();
    Vector2D eTemp = et.position;
    eTemp.x += es.dst.w;
    eTemp.y += es.dst.h;

    for (auto& e: world.getEntities()) {
        if (e->hasComponent<Transform>() && e->hasComponent<EnemyTag>() && e->hasComponent<Sprite>()) {
            auto& t = e->getComponent<Transform>();
            auto& s = e->getComponent<Sprite>();
            Vector2D temp = t.position;
            temp.x += s.dst.w;
            temp.y += s.dst.h;
            if ((temp - eTemp).length() <= distance) {
                collidables.push_back(e.get());
            }
        }
    }
    return collidables;
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
