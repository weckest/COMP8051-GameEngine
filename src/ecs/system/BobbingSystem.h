//
// Created by Weckest on 2026-02-27.
//

#ifndef INC_8051TUTORIAL_BOBBINGSYSTEM_H
#define INC_8051TUTORIAL_BOBBINGSYSTEM_H
#include <complex>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class BobbingSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<ItemTag>() && e->hasComponent<Transform>()) {
                auto& t = e->getComponent<Transform>();
                auto& i = e->getComponent<ItemTag>();

                i.time += dt;
                float speed = 2.0f;
                t.position.y = t.position.y + (std::sin(i.time * speed) - std::sin((i.time - dt)  * speed)) * 5.0f;
            }
        }
    }
};

#endif //INC_8051TUTORIAL_BOBBINGSYSTEM_H