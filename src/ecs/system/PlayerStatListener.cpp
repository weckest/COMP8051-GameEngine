//
// Created by beckh on 2026-03-11.
//

#include "PlayerStatListener.h"

#include <World.h>

PlayerStatListener::PlayerStatListener(World &world) : world(world) {
    world.getEventManager().subscribe([this](const BaseEvent e) {

        if (e.type != EventType::CalculateStats) {
            return;
        }

        else {
            const auto& statEvent = static_cast<const CalculateStatsEvent&>(e);
            applyChanges(statEvent);
        }
    });
}
void PlayerStatListener::applyChanges(const CalculateStatsEvent &e) {

    //Get player Entity.
    Entity* player = nullptr;
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>()) {
            player = entity.get();
            break;
        }
    }

    if (!player) return;

    //Get Item list.

    auto& stats = player->getComponent<PlayerTag>();
    auto& itemList = player->getComponent<ItemList>().items;

    //Formula for applying changes to player stats

    //Reset Player stats as a baseline



    stats.damageModifier=1;
    stats.aoeModifier = 1;
    stats.speedModifier = 1;
    stats.xpModifier = 1;
    stats.fireRateModifier = 1;
    stats.playerSizeModifier = 1;
    stats.projectileSizeModifier = 1;

    for (const auto& item: itemList) {
        int level = item.itemLevel;


    }
}


