//
// Created by Weckest on 2026-04-02.
//

#include "HUDSystem.h"

#include "Game.h"

void HUDSystem::update(const std::vector<std::unique_ptr<Entity> > &entities) {
    Entity* playerEntity = nullptr;

    //find player
    for (auto& e : entities) {
        if (e->hasComponent<PlayerTag>()) {
            playerEntity = e.get();
            break;
        }
    }

    if (!playerEntity) return;
    auto& playerTransform = playerEntity->getComponent<Transform>();

    for (auto& e : entities) {

        if (e->hasComponent<Label>()) {
            auto& label = e.get()->getComponent<Label>();

            //update player position label
            if (label.type == LabelType::PlayerPosition) {
                std::stringstream ss;
                ss << "Player Position: " << playerTransform.position;
                label.text = ss.str();
                label.dirty = true;
            }

            if (label.type == LabelType::PlayerPoints) {
                std::stringstream ss;
                ss << "Points: " << Game::gameState.points;
                label.text = ss.str();
                label.dirty = true;
            }
        }
    }
}
