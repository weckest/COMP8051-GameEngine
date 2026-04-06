//
// Created by Weckest on 2026-02-04.
//

#ifndef INC_8051TUTORIAL_COLLISIONSYSTEM_H
#define INC_8051TUTORIAL_COLLISIONSYSTEM_H
#include <algorithm>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

#include "Entity.h"
#include "Timer.h"

using CollisionKey = std::pair<Entity*, Entity*>;

//forward declaring
class World;

struct CollisionKeyHash {
    size_t operator()(const CollisionKey& k) const {
        return std::hash<Entity*>()(k.first) ^ std::hash<Entity*>()(k.second);
    }
};

class CollisionSystem {
public:
    CollisionSystem(World& world);
    void update(World &world, Timer &timer);

    std::unordered_set<CollisionKey, CollisionKeyHash> activeCollisions;
    static std::vector<Entity*> getAllWithin(World& world, Entity& entity, float distance);
    static Entity* getClosestEntity(World &world, Entity &entity, float distance);
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
    CollisionKey makeKey(Entity* entityA, Entity* entityB) {
        return std::minmax(entityA, entityB); //automatically orders our pair so smaller number (memory address) is ordered first
    }
};

#endif //INC_8051TUTORIAL_COLLISIONSYSTEM_H