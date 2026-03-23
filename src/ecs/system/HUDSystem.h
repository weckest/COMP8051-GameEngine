//
// Created by migue on 2026-03-22.
//

#ifndef INC_8051TUTORIAL_HUDSYSTEM_H
#define INC_8051TUTORIAL_HUDSYSTEM_H
#include <memory>
#include <sstream>
#include <vector>

#include "Component.h"
#include "Entity.h"

class HUDSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        //find player
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) return;
        auto& playerTransform = playerEntity->getComponent<Transform>();

        for (auto& e : entities) {

            if (e->hasComponent<Label>()) {
                auto& label = e.get()->getComponent<Label>();


                //update player position label
                if (label.type == LabelType::PlayerPosition) {
                    std::stringstream ss;
                    ss << "Player Position: " << playerTransform.position;
                    label.text = ss.str();
                    label.dirty = true;
                }
            }

        }


    }
};

#endif //INC_8051TUTORIAL_HUDSYSTEM_H