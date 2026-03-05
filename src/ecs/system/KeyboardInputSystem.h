//
// Created by Weckest on 2026-01-28.
//

#ifndef INC_8051TUTORIAL_KEYBOARDINPUTSYSTEM_H
#define INC_8051TUTORIAL_KEYBOARDINPUTSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World;

class KeyBoardInputSystem {
public:
    void update(World& world, const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event);
};

#endif //INC_8051TUTORIAL_KEYBOARDINPUTSYSTEM_H