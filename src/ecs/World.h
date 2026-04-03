//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_WORLD_H
#define INC_8051TUTORIAL_WORLD_H
#include <iostream>
#include <memory>
#include <vector>
#include <manager/WeaponManager.h>

#include "AnimationSystem.h"
#include "BobbingSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "DebugRenderSystem.h"
#include "DestructionSystem.h"
#include "EffectAnimationSystem.h"
#include "EffectSystem.h"
#include "EnemyMovementSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "GameStateSystem.h"
#include "GridSystem.h"
#include "HeathSystem.h"
#include "HUDSystem.h"
#include "KeyboardInputSystem.h"
#include "LevelUpHandler.h"
#include "LevelUpSystem.hpp"
#include "LifetimeSystem.h"
#include "event/EventManager.h"
#include "GameOverSystem.h"
#include "Map.h"
#include "MouseInputSystem.h"
#include "MovementSystem.h"
#include "OverlayRenderSystem.h"
#include "PickUpSystem.h"
#include "PlayerStatListener.h"
#include "PreRenderSystem.h"
#include "RenderSystem.h"
#include "SpawnerSystem.h"
#include "SpawnTimerSystem.h"
#include "WeaponFireSystem.h"
#include "Timer.h"
#include "UIRenderSystem.h"
#include "data/DebugState.h"
#include "event/AudioEventQueue.h"
#include "manager/ItemManager.hpp"
#include "scene/SceneType.h"

//could also be called EntityManager
class World {
    bool isPaused = false;
    Map map;
    Timer timer;
    DebugState debugState;
    Entity* player;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::vector<std::vector<Entity*>>> entityGrid;
    int rows = 25;
    int cols = 25;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    EventManager eventManager;
    ItemManager itemManager;
    LifetimeSystem lifetimeSystem;
    WeaponManager weaponManager;
    GameStateSystem gameStateSystem;
    RenderSystem renderSystem{*this};
    MovementSystem movementSystem{*this};
    KeyBoardInputSystem keyboardInputSystem;
    GridSystem gridSystem{*this};
    CollisionSystem collisionSystem{*this};
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    SpawnTimerSystem spawnTimerSystem;
    DestructionSystem destructionSystem;
    EnemyMovementSystem enemyMovementSystem;
    BobbingSystem bobbingSystem;
    EffectSystem effectSystem;
    EffectAnimationSystem effectAnimationSystem;
    EventResponseSystem eventResponseSystem{*this};
    GameOverSystem gameOverSystem;
    SpawnerSystem spawnerSystem{*this};
    LevelUpSystem levelUpSystem;
    HealthSystem healthSystem;
    WeaponFireSystem weaponFireSystem;
    LevelUpHandler levelUpHandler{*this};
    UIRenderSystem uiRenderSystem;
    MouseInputSystem mouseInputSystem;
    PlayerStatListener playerStatListener{*this};
    PickUpSystem pickUpSystem;
    DebugRenderSystem debugRenderSystem{*this};
    HUDSystem hudSystem;
    PreRenderSystem preRenderSystem;
    AudioEventQueue audioEventQueue;
    OverlayRenderSystem overlayRenderSystem;


public:
    World();

    void update(float dt, SDL_Event& event, SceneType sceneType, SDL_Renderer* renderer) {

        if (sceneType == SceneType::MainMenu) {
            //pass through
        } else if (sceneType == SceneType::GameOver) {
            gameOverSystem.update(*this, event);
        }
        else if (isPaused) {
            keyboardInputSystem.update(*this, entities, event);
        } else {
            timer.startTimer("update");
            keyboardInputSystem.update(*this, entities, event);
            gameStateSystem.update(entities, dt);
            bobbingSystem.update(entities, dt);
            timer.startTimer("movement");
            movementSystem.update(entities, dt);
            timer.stopTimer("movement");
            if (!debugState.stopEnemies) {
                enemyMovementSystem.update(entities, dt);
            }
            timer.startTimer("pickUp");
            pickUpSystem.update(entities, *this);
            timer.stopTimer("pickUp");
            timer.startTimer("colliders");
            timer.startTimer("grid");
            // std::cout << "Main Grid Update" << std::endl;
            gridSystem.update(entityGrid, entities, *this);
            timer.stopTimer("grid");
            timer.startTimer("collision");
            collisionSystem.update(*this, timer);
            timer.stopTimer("collision");
            timer.stopTimer("colliders");
            effectSystem.update(entities, dt);
            animationSystem.update(entities, dt);
            effectAnimationSystem.update(entities,dt);
            cameraSystem.update(entities);
            if (!debugState.stopSpawn) {
                weaponFireSystem.update(*this, dt);
                spawnTimerSystem.update(entities, dt);
            }
            destructionSystem.update(entities);
            levelUpSystem.update(entities, *this);
            healthSystem.update(entities, *this);
            lifetimeSystem.update(entities, dt);
            hudSystem.update(entities);
            timer.stopTimer("update");
        }
        bobbingSystem.update(entities, dt);
        mouseInputSystem.update(*this, event,renderer);
        audioEventQueue.process(); //process all audio events
        timer.startTimer("prerender");
        preRenderSystem.update(entities);
        timer.stopTimer("prerender");


        synchronizeEntities();
        cleanup();
        if (sceneType != SceneType::MainMenu && !isPaused) {
            // std::cout << "Grid Update" << std::endl;
            gridSystem.update(entityGrid, entities, *this);
        }
    }

    void render() {

        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                auto& camera = e->getComponent<Camera>();

                map.draw(camera);
                // if (debugState.debug && debugState.grid) {
                //     gridSystem.draw(e->getComponent<Camera>());
                //     gridSystem.updateCellLabels(*this);
                // }
            }
        }

        timer.startTimer("render humanoids");
        renderSystem.render(entities, true);
        timer.stopTimer("render humanoids");

        //perform handover
        if (!overlayRenderSystem.hasData) {
            overlayRenderSystem.receiveData(map.overlayTileData, map.width, map.height, map.scale, map.tileset);
        }
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                auto& camera = e->getComponent<Camera>();

                overlayRenderSystem.draw(camera);
                if (debugState.debug && debugState.grid) {
                    gridSystem.draw(e->getComponent<Camera>());
                    gridSystem.updateCellLabels(*this);
                }
            }
        }

        timer.startTimer("render others");
        renderSystem.render(entities, false);
        timer.stopTimer("render others");

        timer.startTimer("debug");
        if (debugState.debug) {
            debugRenderSystem.render(entities, debugState);
        }
        timer.stopTimer("debug");
        uiRenderSystem.render(entities);

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
                    //remove the eaantity from the grid
                    eventManager.emit(DeathEvent{e.get()});
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

    AudioEventQueue& getAudioEventQueue() {
        return audioEventQueue;
    }

    ItemManager& getItemManager() {
        return itemManager;
    }
    WeaponManager& getWeaponManager() {
        return weaponManager;
    }

    CollisionSystem& getCollisionSystem() {
        return collisionSystem;
    }

    void resetGame()
    {
        //TODO: Handle reset logic somewhere reasonable
    }

    Map& getMap() {return map;}

    DebugState& getDebugState() {return debugState;}

    Entity* getPlayer() {
        return player;
    }

    void setPlayer(Entity* player) {
        this->player = player;
    }

    Timer& getTimer() {
        return timer;
    }

    void togglePaused() {
        isPaused = !isPaused;
    }

    void initDebug();

};

#endif //INC_8051TUTORIAL_WORLD_H