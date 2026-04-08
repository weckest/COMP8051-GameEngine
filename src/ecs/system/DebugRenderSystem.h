//
// Created by Weckest on 2026-03-18.
//

#ifndef INC_8051TUTORIAL_DEBUGRENDERSYSTEM_H
#define INC_8051TUTORIAL_DEBUGRENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "data/DebugState.h"

class Entity;
class World;

class DebugRenderSystem {
    World& world;
public:
    DebugRenderSystem(World& world): world(world) {}

    void render(const std::vector<std::unique_ptr<Entity>>& entities, DebugState debugState, float dt, float at);
    void initDebugLabel();
    void updateDebugLabel(Entity& entity, float dt, float at);
    Entity& createChildDebugLabel(Entity& parent, LabelType type, Vector2D position, std::string key = "");
};

#endif //INC_8051TUTORIAL_DEBUGRENDERSYSTEM_H