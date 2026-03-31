//
// Created by Weckest on 2026-03-31.
//

#include "Game.h"
#include "HeathSystem.h"
#include "World.h"

void HealthSystem::update(const std::vector<std::unique_ptr<Entity>>& entities, World &world) {
    Entity* player = world.getPlayer();

    if (!player) return;
    auto& pt = player->getComponent<PlayerTag>();

    for (auto& e: entities) {
        if (e->hasComponent<HealthBar>()) {
            auto& c = e->getComponent<Children>().children.front();
            if (c->hasComponent<Sprite>()) {
                auto& s = c->getComponent<Sprite>();
                s.dst.w = (200.0f - 2.5f * 2) * (Game::gameState.playerHealth / 100.0f);
            }
        }
    }
}
