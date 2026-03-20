//
// Created by Weckest on 2026-03-05.
//

#ifndef INC_8051TUTORIAL_SPAWNERSYSTEM_H
#define INC_8051TUTORIAL_SPAWNERSYSTEM_H
#include <functional>
#include <string>
#include <unordered_map>

#include "Component.h"

class World;

class SpawnerSystem {
public:
    SpawnerSystem(World& world);
    void SpawnRandomItem(const Vector2D &position);

private:
    std::unordered_map<std::string, std::function<void(Vector2D)>> spawners;

};

#endif //INC_8051TUTORIAL_SPAWNERSYSTEM_H