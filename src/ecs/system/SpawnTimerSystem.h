//
// Created by Weckest on 2026-02-25.
//

#ifndef INC_8051TUTORIAL_SPAWNTIMERSYSTEM_H
#define INC_8051TUTORIAL_SPAWNTIMERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class SpawnTimerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        Entity* camera = nullptr;
        Entity* player = nullptr;

        for (auto& e: entities) {
            if (e->hasComponent<Camera>()) {
                camera = e.get();
            }
            if (e->hasComponent<PlayerTag>()) {
                player = e.get();
            }
        }
        if (!camera) return;
        if (!player) return;
        auto& cam = camera->getComponent<Camera>();
        auto& pt = player->getComponent<Transform>();
        for (auto& entity: entities) {
            if (entity->hasComponent<TimedSpawner>() && entity->hasComponent<Transform>()) {
                auto& spawner = entity->getComponent<TimedSpawner>();
                auto& transform = entity->getComponent<Transform>();

                float tX = transform.position.x;
                float tY = transform.position.y;

                if ((tX > cam.view.x && tX < cam.view.x + cam.view.w) && (tY > cam.view.y && tY < cam.view.y + cam.view.h)
                    && !entity->hasComponent<PlayerTag>() && !entity->hasComponent<EnemyTag>()) {
                    continue;
                }

                spawner.timer -= dt;

                if (spawner.timer <= 0) {
                    spawner.timer = spawner.spawnInterval;
                    spawner.spawnCallback(); //create entity inside here
                }
            }
        }
    }
};

#endif //INC_8051TUTORIAL_SPAWNTIMERSYSTEM_H