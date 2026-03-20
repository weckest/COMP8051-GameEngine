//
// Created by Weckest on 2026-03-18.
//

#ifndef INC_8051TUTORIAL_PICKUPSYSTEM_H
#define INC_8051TUTORIAL_PICKUPSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World;


class PickUpSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, World& world);
};


#endif //INC_8051TUTORIAL_PICKUPSYSTEM_H