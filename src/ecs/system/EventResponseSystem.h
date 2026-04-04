//
// Created by Weckest on 2026-03-04.
//

#ifndef INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H
#define INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H
#include <functional>
#include <string>

#include "event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
public:
    EventResponseSystem(World& world);

private:
    bool checkTagsFor(const char* ATag, const char* BTag, std::string tag);

    //collisions
    void onCollision(const CollisionEvent& e, const char* ATag, const char* BTag, World& world);
    bool getCollisionEntities(const CollisionEvent& e, const char* ATag, const char* BTag, Entity*& entityA, Entity*& entityB);


    //player action
    void onPlayerAction(const PlayerActionEvent& e,
        const std::function<void(Entity* player, PlayerAction action)>& callback);

    void onMouseInteraction(const MouseInteractionEvent& e, World& world);
};

#endif //INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H