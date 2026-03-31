//
// Created by Weckest on 2026-03-09.
//

#include "MovementSystem.h"
#include "World.h"

MovementSystem::MovementSystem(World& world) : world(world) {
    world.getEventManager().subscribe([&world](const BaseEvent& e) {
       if (e.type != EventType::Magnet) return;
        Entity* player = world.getPlayer();
        auto& pT = player->getComponent<Transform>();

        for (auto& entity : world.getEntities()) {
            if (entity->hasComponent<ItemTag>() && entity->hasComponent<Transform>()) {
                auto& t = entity->getComponent<Transform>();
                entity->addComponent<Velocity>(pT.position - t.position, 500.0f);
            }
        }
    });
}

void MovementSystem::update(std::vector<std::unique_ptr<Entity>> &entities, float dt) {
    for (auto& entity : entities) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>() && !entity->hasComponent<EnemyTag>()) {
            auto& t = entity->getComponent<Transform>();
            auto& v = entity->getComponent<Velocity>();

            //track previous frames position
            t.oldPosition = t.position;

            Vector2D directionVec = v.direction;

            if (v.direction.x != 0.0f) {
                v.facingRight = v.direction.x > 0.0f;
            }

            if (entity->hasComponent<ItemTag>()) {
                directionVec = world.getPlayer()->getComponent<Transform>().position - t.position;
            }

            //normalizing
            directionVec.normalize();

            if (entity->hasComponent<PlayerTag>()) {
                directionVec *= entity->getComponent<Stats>().speedModifier;
            }


            Vector2D velocityVec1 = directionVec * v.speed;

            t.position += velocityVec1 * dt;
        }
    }
}
