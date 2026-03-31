//
// Created by Weckest on 2026-03-31.
//

#ifndef INC_8051TUTORIAL_GAMESTATESYSTEM_H
#define INC_8051TUTORIAL_GAMESTATESYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class GameStateSystem {
    float totalTime = 0.0f;
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        totalTime += dt;
        for (auto& e: entities) {


            //speed up the spawners as time goes on
            if (e->hasComponent<TimedSpawner>() && !e->hasComponent<PlayerTag>()) {
                auto& s = e->getComponent<TimedSpawner>();
                s.spawnInterval *= (1.0f-(dt / 1000.0f));
            }
        }
    }
};

#endif //INC_8051TUTORIAL_GAMESTATESYSTEM_H