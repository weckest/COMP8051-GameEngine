//
// Created by beckh on 2026-03-09.
//

#include "LevelUpSystem.hpp"
#include "World.h"

void LevelUpSystem::update(const std::vector<std::unique_ptr<Entity>>& entities, World& world) {

    //Get player
    Entity* playerEntity = nullptr;
    Entity* camera = nullptr;

    //find the player
    for (auto& e : entities) {
        if (e->hasComponent<PlayerTag>()) {
            playerEntity = e.get();
        }
        if (e->hasComponent<Camera>()) {
            camera = e.get();
        }
    }
    if (!playerEntity) return;
    if (!camera) return;


    //Gets the stats. If xp is more than threshold, level up, subtract xp from player.
    auto& stats = playerEntity->getComponent<PlayerTag>();
    auto& cam = camera->getComponent<Camera>();

    if (world.getDebugState().debug && world.getDebugState().level) {
        std::cout << "Debug Level" <<  stats.level << std::endl;
    }

    if (stats.xp >= xpToLevelUp) {
        stats.level++;
        stats.xp -= xpToLevelUp;
        xpToLevelUp+=200;
        world.getEventManager().emit(LevelUpEvent{stats.level});
        //std::cout << "LEVEL UP!" << std::endl;
    }

    for (auto& e: entities) {
        if (e->hasComponent<LevelUpBar>() && e->hasComponent<Children>()) {
            auto& bar = e->getComponent<Children>().children.front();
            if (bar->hasComponent<Sprite>()) {
                auto& sprite = bar->getComponent<Sprite>();
                sprite.dst.w = (cam.view.w - 10.0f) * stats.xp / xpToLevelUp;
            }
        }
    }
}

