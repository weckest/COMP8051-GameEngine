//
// Created by Weckest on 2026-03-05.
//

#include "RenderSystem.h"
#include "World.h"

RenderSystem::RenderSystem(World &world) : world(world) {
    world.getEventManager().subscribe(
        [this](const BaseEvent &e) {
            if (e.type != EventType::ToggleDebug) return;

            debug = !debug;

        });
} ;
