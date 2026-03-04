//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_MOVEMENTSYSTEM_H
#define INC_8051TUTORIAL_MOVEMENTSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class MovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
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
};

#endif //INC_8051TUTORIAL_MOVEMENTSYSTEM_H