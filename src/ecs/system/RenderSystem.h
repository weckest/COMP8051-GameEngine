//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_RENDERSYSTEM_H
#define INC_8051TUTORIAL_RENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "TextureManager.h"
#include <iostream>

#include "SDL3/SDL_render.h"

class World;

class RenderSystem {
    World& world;
public:
    RenderSystem(World& world) : world(world) {}
    void render(const std::vector<std::unique_ptr<Entity>>& entities);
};

#endif //INC_8051TUTORIAL_RENDERSYSTEM_H