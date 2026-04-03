//
// Created by Weckest on 2026-03-31.
//

#ifndef INC_8051TUTORIAL_GAMESTATESYSTEM_H
#define INC_8051TUTORIAL_GAMESTATESYSTEM_H
#include <memory>
#include <vector>

#include "Entity.h"

class GameStateSystem {
public:
    GameStateSystem() {}
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt);
};

#endif //INC_8051TUTORIAL_GAMESTATESYSTEM_H