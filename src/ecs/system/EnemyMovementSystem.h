//
// Created by Weckest on 2026-02-26.
//

#ifndef INC_8051TUTORIAL_ENEMYMOVEMENTSYSTEM_H
#define INC_8051TUTORIAL_ENEMYMOVEMENTSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"


namespace tinyxml2 {
    struct Entity;
}

class EnemyMovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {

        Entity* player = nullptr;

        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                player = e.get();
            }
        }

        if (!player) return;

        for (auto& e : entities) {
            if (e->hasComponent<EnemyTag>() && e->hasComponent<Velocity>() && e->hasComponent<Transform>()) {
                auto& t = e->getComponent<Transform>();
                auto& v = e->getComponent<Velocity>();

                //track previous frames position
                t.oldPosition = t.position;

                v.direction = player->getComponent<Transform>().position - t.position;

                Vector2D directionVec = v.direction;
                //normalizing
                directionVec.normalize();

                Vector2D velocityVec1 = directionVec * v.speed;

                t.position += velocityVec1 * dt;
            }
        }
    }
};

#endif //INC_8051TUTORIAL_ENEMYMOVEMENTSYSTEM_H