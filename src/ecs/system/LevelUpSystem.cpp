//
// Created by beckh on 2026-03-09.
//

#include "LevelUpSystem.hpp"
#include "World.h"

void LevelUpSystem::update(const std::vector<std::unique_ptr<Entity>>& entities, World& world) {

    //Get player
    Entity* playerEntity = nullptr;

    //find the player
    for (auto& e : entities) {
        if (e->hasComponent<PlayerTag>()) {
            playerEntity = e.get();
            break;
        }
    }
    if (!playerEntity) return;


    //Gets the stats. If xp is more than threshold, level up, subtract xp from player.
    auto& stats = playerEntity->getComponent<PlayerTag>();

    if (stats.xp >= xpToLevelUp) {
        stats.level++;
        stats.xp -= xpToLevelUp;
        world.getEventManager().emit(LevelUpEvent{stats.level});
        std::cout << "LEVEL UP!" << std::endl;

    }
}

