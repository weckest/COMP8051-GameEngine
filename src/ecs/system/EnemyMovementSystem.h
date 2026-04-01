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
                auto& s = e->getComponent<Sprite>();
                auto& pt = player->getComponent<Transform>();
                auto& ps = player->getComponent<Sprite>();
                auto& v = e->getComponent<Velocity>();

                //track previous frames position
                t.oldPosition = t.position;

                Vector2D playerCenter = pt.position + Vector2D(ps.dst.w / 2, ps.dst.h / 2);
                Vector2D enemyCenter = t.position + Vector2D(s.dst.w / 2, s.dst.h / 2);

                v.direction = playerCenter - enemyCenter;

                Vector2D directionVec = v.direction;
                //normalizing
                directionVec.normalize();

                Vector2D velocityVec1 = directionVec * v.speed;
                // std::cout << "Moving: " << e << " " << (velocityVec1 * dt).x << " " << (velocityVec1 * dt).y << std::endl;

                t.position += velocityVec1 * dt;
            }
        }
    }
};

#endif //INC_8051TUTORIAL_ENEMYMOVEMENTSYSTEM_H