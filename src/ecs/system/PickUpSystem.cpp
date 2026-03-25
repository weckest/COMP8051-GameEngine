//
// Created by Weckest on 2026-03-18.
//

#include "PickUpSystem.h"

#include "Game.h"
#include "World.h"

void PickUpSystem::update(std::vector<std::unique_ptr<Entity>>& entities, World& world) {
    Entity* player = world.getPlayer();
    auto& pT = player->getComponent<Transform>();
    auto& pS = player->getComponent<Sprite>();
    auto& pStats = player->getComponent<Stats>();
    Vector2D pCenter = pT.position + Vector2D{(pS.dst.w / 2), (pS.dst.h / 2)};

    for (auto& e : entities) {
        if (e->hasComponent<ItemTag>() && e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
            auto& eS = e->getComponent<Sprite>();
            auto& eT = e->getComponent<Transform>();
            Vector2D eCenter = eT.position + Vector2D{eS.dst.w / 2, eS.dst.h / 2};
            // std::cout << (pCenter - eCenter).length() << ", " << pStats.aoeModifier * 64 << std::endl;
            if ((pCenter - eCenter).length() <= pStats.aoeModifier * 64) {

                if (e->hasComponent<MagnetTag>()) {
                    world.getEventManager().emit(MagnetEvent{});
                    world.getEventManager().emit(DeathEvent(e.get()));
                    e->destroy();
                    continue;
                }

                if (e->hasComponent<FoodTag>()) {
                    player->getComponent<Health>().currentHealth += e->getComponent<FoodTag>().heal;
                    Game::gameState.playerHealth = player->getComponent<Health>().currentHealth;
                    world.getEventManager().emit(DeathEvent(e.get()));
                    e->destroy();
                    continue;
                }

                ///TEST CODE. NEED TO REMOVE LATER
                auto& stats = player->getComponent<PlayerTag>();
                auto& modifiers = player->getComponent<Stats>();

                stats.xp += (10 * modifiers.xpModifier);

                world.getAudioEventQueue().push(std::make_unique<AudioEvent>("collect"));

                ///END OF TEST CODE
                world.getEventManager().emit(DeathEvent(e.get()));
                e->destroy();
            }
        }
    }
}
