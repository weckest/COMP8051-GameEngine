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
#include "GridSystem.h"
#include "KeyboardInputSystem.h"
#include "LevelUpSystem.hpp"
#include "event/EventManager.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "SpawnerSystem.h"
#include "SpawnTimerSystem.h"
#include "WeaponFireSystem.h"
#include "Timer.h"
#include "data/DebugState.h"
#include "scene/SceneType.h"

//could also be called EntityManager
class World {
    Map map;
    Timer timer;
    DebugState debugState;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::vector<std::vector<Entity*>>> entityGrid;
    int rows = 3;
    int cols = 5;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    EventManager eventManager;
    RenderSystem renderSystem{*this};
    MovementSystem movementSystem{*this};
    KeyBoardInputSystem keyboardInputSystem;
    GridSystem gridSystem{*this};
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    SpawnTimerSystem spawnTimerSystem;
    DestructionSystem destructionSystem;
    EnemyMovementSystem enemyMovementSystem;
    BobbingSystem bobbingSystem;
    EffectSystem effectSystem;
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;
    SpawnerSystem spawnerSystem{*this};
    LevelUpSystem levelUpSystem;
    WeaponFireSystem weaponFireSystem;


public:
    World() {
        entityGrid = std::vector<std::vector<std::vector<Entity*>>>(
            rows,
            std::vector<std::vector<Entity*>>(cols)
        );

        eventManager.subscribe(
            [this](const BaseEvent& e) {
                if (e.type != EventType::KeyPress) return;
                const auto& keyPress = static_cast<const KeyPressEvent&>(e);

                switch (keyPress.keyCode) {
                    case SDLK_T:
                        debugState.timer = !debugState.timer;
                        break;
                    default:
                        break;
                }
        });
    }
    void update(float dt, SDL_Event& event, SceneType sceneType) {

        if (sceneType == SceneType::MainMenu) {
            //main menu system
            mainMenuSystem.update(event);
        } else {
            timer.startTimer("update");
            keyboardInputSystem.update(*this, entities, event);
            bobbingSystem.update(entities, dt);
            movementSystem.update(entities, dt);
            enemyMovementSystem.update(entities, dt);
            spawnTimerSystem.update(entities, dt);
            timer.startTimer("colliders");
            timer.startTimer("grid");
            gridSystem.update(entityGrid, entities, *this);
            timer.stopTimer("grid");
            timer.startTimer("collision");
            collisionSystem.update(*this);
            timer.stopTimer("collision");
            timer.stopTimer("colliders");
            effectSystem.update(entities, dt);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            destructionSystem.update(entities);
            levelUpSystem.update(entities, *this);
            weaponFireSystem.update(*this, dt);
            timer.stopTimer("update");
        }
        synchronizeEntities();
        cleanup();
    }

    void render() {

        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                map.draw(e->getComponent<Camera>());
                if (renderSystem.isDebug()) {
                    gridSystem.draw(e->getComponent<Camera>());
                }
            }
        }

        renderSystem.render(entities);

        if (renderSystem.isDebug() && debugState.timer) {
            timer.printResults();
        }
    }

    Entity& createEntity() {
        //we use emplace instead of push so we dont create a copy
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createEntity(Entity& e) {
        entities.emplace_back(std::make_unique<Entity>(e));
        return *entities.back();
    }

    Entity& createDeferredEntity() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();
    }

    Entity& createDeferredEntity(Entity e) {
        deferredEntities.emplace_back(std::make_unique<Entity>(e));
        return *deferredEntities.back();
    }

    static Entity createTempEntity() {
        return *std::make_unique<Entity>();
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }

    std::vector<std::vector<std::vector<Entity*>>>& getEntityGrid() {
        return entityGrid;
    }

    void cleanup() {
        //use lambda predicate to remove all inactive entities
        std::erase_if(
            entities,
            [this](std::unique_ptr<Entity>& e) {
                if (!e->isActive()) {
                    //print the entity address we are cleaning up
                    // std::cout << "Entity " << e << " destroyed" << std::endl;
                    //remove the entity from the grid
                    eventManager.emit(DeathEvent{&*e});
                    return !e->isActive();
                }
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