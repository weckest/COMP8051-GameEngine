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
    SpawnEntity,
    Death,
    GridDebug,
    LevelUp,
    KeyPress,
    MouseInteraction
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

struct DeathEvent : BaseEvent {
    Entity* entity = nullptr;
    DeathEvent(Entity* entity) : entity(entity) {
        type = EventType::Death;
    }
};

struct GridDebugEvent : BaseEvent {
    Entity* entity = nullptr;
    GridDebugEvent(Entity* entity) : entity(entity) {
        type = EventType::GridDebug;
    }
};

struct LevelUpEvent : BaseEvent {
    int newLevel;
    LevelUpEvent(int lvl) : newLevel(lvl) {
        type = EventType::LevelUp;
    }
};

struct KeyPressEvent : BaseEvent {
    SDL_Keycode keyCode;
    KeyPressEvent(SDL_Keycode keyCode) : keyCode(keyCode) {
        type = EventType::KeyPress;
    }
};

enum class MouseInteractionState {
    Pressed, Released, Cancel
};

struct MouseInteractionEvent : BaseEvent {
    Entity* entity = nullptr;
    MouseInteractionState state{};
    MouseInteractionEvent(Entity* entity, MouseInteractionState state) : entity(entity), state(state) {
        type = EventType::MouseInteraction;
    }
};



#endif //INC_8051TUTORIAL_BASEEVENT_H