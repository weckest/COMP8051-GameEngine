//
// Created by beckh on 2026-03-11.
//

#include "LevelUpHandler.h"

#include <World.h>


//Subscribes to the Level Up event, which calls onLevelUp when emitted.
LevelUpHandler::LevelUpHandler(World &world) {
    world.getEventManager().subscribe([this](const BaseEvent& e) {

        if (e.type!= EventType::LevelUp) {
            return;
        } else {
            const auto& lvlEvent = static_cast<const LevelUpEvent&>(e);
            onLevelUp(lvlEvent);
        }
    });
}

void LevelUpHandler::onLevelUp(const LevelUpEvent& e) {
    std::cout << "Level Up Handler Success"<< std::endl;
}
