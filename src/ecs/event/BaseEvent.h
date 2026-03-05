//
// Created by Weckest on 2026-03-04.
//

#ifndef INC_8051TUTORIAL_BASEEVENT_H
#define INC_8051TUTORIAL_BASEEVENT_H
#include "Component.h"
#include "Vector2D.h"

class Entity;

enum class EventType {
    Collision,
    PlayerAction,
    ToggleDebug,
    SpawnEntity
};

struct BaseEvent {
    EventType type;
    virtual ~BaseEvent() = default;
};

enum class CollisionState {
    Enter, Stay, Exit
};

struct CollisionEvent : BaseEvent {
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;
    CollisionState state{};
    CollisionEvent(Entity* entityA, Entity* entityB, CollisionState state) : entityA(entityA), entityB(entityB), state(state) {
        type = EventType::Collision;
    }
};

enum class PlayerAction {
    Attack,
    Interact,
    useItem,
    Jump
};

struct PlayerActionEvent : BaseEvent {
    Entity* player = nullptr;
    PlayerAction action{};
    PlayerActionEvent(Entity* player, PlayerAction action) : player(player), action(action) {
        type = EventType::PlayerAction;
    }
};

struct ToggleDebugEvent : BaseEvent {
    ToggleDebugEvent() : BaseEvent() {
        type = EventType::ToggleDebug;
    }
};

struct SpawnPrefabEvent : BaseEvent {
    const char*  name;
    Transform transform;
    SpawnPrefabEvent(const char* name, const Transform &transform) : name(name), transform(transform) {
        type = EventType::SpawnEntity;
    }
};

#endif //INC_8051TUTORIAL_BASEEVENT_H