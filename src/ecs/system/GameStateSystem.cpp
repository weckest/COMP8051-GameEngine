//
// Created by Weckest on 2026-04-02.
//

#include "GameStateSystem.h"
#include "Game.h"

void GameStateSystem::update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
    Game::gameState.time += dt;
    for (auto& e: entities) {

        //speed up the spawners as time goes on
        if (e->hasComponent<TimedSpawner>() && !e->hasComponent<PlayerTag>()) {
            auto& s = e->getComponent<TimedSpawner>();
            s.spawnInterval *= (1.0f-(dt / 1000.0f));
        }
    }
}
