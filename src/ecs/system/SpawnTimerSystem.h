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
        for (auto& entity: entities) {
            if (entity->hasComponent<TimedSpawner>()) {
                auto& spawner = entity->getComponent<TimedSpawner>();

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