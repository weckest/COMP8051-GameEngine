//
// Created by Weckest on 2026-02-04.
//

#ifndef INC_8051TUTORIAL_COLLISIONSYSTEM_H
#define INC_8051TUTORIAL_COLLISIONSYSTEM_H
#include <memory>
#include <vector>

class Entity;
//forward declaring
class World;

class CollisionSystem {
public:
    void update(World& world);
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
};

#endif //INC_8051TUTORIAL_COLLISIONSYSTEM_H