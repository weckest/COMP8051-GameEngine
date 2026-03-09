//
// Created by Weckest on 2026-03-09.
//

#include "MovementSystem.h"
#include "World.h"

MovementSystem::MovementSystem(World& world) : world(world) {}

void MovementSystem::update(std::vector<std::unique_ptr<Entity>> &entities, float dt) {
    for (auto& entity : entities) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>() && !entity->hasComponent<EnemyTag>()) {
            auto& t = entity->getComponent<Transform>();
            auto& v = entity->getComponent<Velocity>();

            //track previous frames position
            t.oldPosition = t.position;

            Vector2D directionVec = v.direction;
            //normalizing
            directionVec.normalize();

            if (entity->hasComponent<PlayerTag>()) {
                directionVec *= entity->getComponent<PlayerTag>().speedModifier;
            }

            Vector2D velocityVec1 = directionVec * v.speed;

            t.position += velocityVec1 * dt;
        }
    }
}
