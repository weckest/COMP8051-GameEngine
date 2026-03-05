//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_WORLD_H
#define INC_8051TUTORIAL_WORLD_H
#include <iostream>
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "BobbingSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "DestructionSystem.h"
#include "EffectSystem.h"
#include "EnemyMovementSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "scene/SceneType.h"


//could also be called EntityManager
class World {
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    KeyBoardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    SpawnTimerSystem spawnTimerSystem;
    DestructionSystem destructionSystem;
    EnemyMovementSystem enemyMovementSystem;
    BobbingSystem bobbingSystem;
    EffectSystem effectSystem;
    EventManager eventManager;
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;


public:
    World() = default;
    void update(float dt, SDL_Event& event, SceneType sceneType) {

        if (sceneType == SceneType::MainMenu) {
            //main menu system
            mainMenuSystem.update(event);
        } else {
            keyboardInputSystem.update(entities, event);
            bobbingSystem.update(entities, dt);
            movementSystem.update(entities, dt);
            enemyMovementSystem.update(entities, dt);
            spawnTimerSystem.update(entities, dt);
            collisionSystem.update(*this);
            effectSystem.update(entities, dt);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            destructionSystem.update(entities);
        }
        synchronizeEntities();
        cleanup();
    }

    void render() {

        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                map.draw(e->getComponent<Camera>());
            }
        }

        renderSystem.render(entities);
    }

    Entity& createEntity() {
        //we use emplace instead of push so we dont create a copy
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createDeferredEntity() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }

    void cleanup() {
        //use lambda predicate to remove all inactive entities
        std::erase_if(
            entities,
            [](std::unique_ptr<Entity>& e) {
                    return !e->isActive();
            }
        );
    }

    void synchronizeEntities() {
        if (!deferredEntities.empty()) {
            //push back all deferred entities to the entities vector
            //using move so we dont create a copy
            std::move(
                deferredEntities.begin(),
                deferredEntities.end(),
                std::back_inserter(entities)
            );
            //clearing the creation buffer
            deferredEntities.clear();
        }
    }

    EventManager& getEventManager() {
        return eventManager;
    }

    Map& getMap() {return map;}

};

#endif //INC_8051TUTORIAL_WORLD_H