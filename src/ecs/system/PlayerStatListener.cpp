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

    auto& stats = player->getComponent<Stats>();
    auto& transform = player->getComponent<Transform>();
    auto& itemList = player->getComponent<ItemList>().items;


    //Reset values of each stat.
    stats.damageModifier = 0.0f;
    stats.speedModifier = 1.0f;
    stats.fireRateModifier = 0.0f;
    stats.playerSizeModifier = 1.0f;
    stats.projectileSizeModifier = 0.0f;
    stats.aoeModifier = 1.0f;
    stats.xpModifier = 1.0f;

    //For each item, apply its stats to the player
    for (const auto& item: itemList) {
        float level = item.itemLevel;
        stats.damageModifier        += (item.damageModifier - 1.0f) * level;
        stats.speedModifier         += (item.speedModifier - 1.0f) * level;
        stats.fireRateModifier      += (item.fireRateModifier - 1.0f) * level;
        stats.playerSizeModifier    += (item.playerSizeModifier - 1.0f) * level;
        stats.projectileSizeModifier+= (item.projectileSizeModifier - 1.0f) * level;
        stats.aoeModifier           += (item.aoeModifier - 1.0f) * level;
        stats.xpModifier            += (item.xpModifier - 1.0f) * level;
    }

    transform.scale = stats.playerSizeModifier;

    std::cout << "Damage mod: " << stats.damageModifier << std::endl;
    std::cout << "Speed mod: " << stats.speedModifier << std::endl;
    std::cout << "fire Rate mod: " << stats.fireRateModifier << std::endl;
    std::cout << "playerSize mod: " << stats.playerSizeModifier << std::endl;
    std::cout << "aoe mod: " << stats.aoeModifier << std::endl;
    std::cout << "xp mod: " << stats.xpModifier << std::endl;









}


