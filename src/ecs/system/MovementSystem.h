//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_MOVEMENTSYSTEM_H
#define INC_8051TUTORIAL_MOVEMENTSYSTEM_H
#include <iostream>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World;

class MovementSystem {
    World& world;
public:
    MovementSystem(World& world);
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt);
};

#endif //INC_8051TUTORIAL_MOVEMENTSYSTEM_H