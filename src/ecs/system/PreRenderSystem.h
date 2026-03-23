//
// Created by migue on 2026-03-22.
//

#ifndef INC_8051TUTORIAL_PRERENDERSYSTEM_H
#define INC_8051TUTORIAL_PRERENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "manager/TextureManager.h"


class PreRenderSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Label>() && entity->getComponent<Label>().dirty) {
                TextureManager::updateLabel(entity->getComponent<Label>());
            }
        }
    }
};


#endif //INC_8051TUTORIAL_PRERENDERSYSTEM_H