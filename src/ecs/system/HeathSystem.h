//
// Created by Weckest on 2026-03-31.
//

#ifndef INC_8051TUTORIAL_HEATHSYSTEM_H
#define INC_8051TUTORIAL_HEATHSYSTEM_H
#include <memory>
#include <vector>

#include "Entity.h"
class World;

class HealthSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, World& world);
};

#endif //INC_8051TUTORIAL_HEATHSYSTEM_H