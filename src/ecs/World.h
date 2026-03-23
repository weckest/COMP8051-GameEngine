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
#include "EffectSystem.h"
#include "EnemyMovementSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "GridSystem.h"
#include "HUDSystem.h"
#include "KeyboardInputSystem.h"
#include "LevelUpHandler.h"
#include "LevelUpSystem.hpp"
#include "event/EventManager.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MouseInputSystem.h"
#include "MovementSystem.h"
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
    int rows = 10;
    int cols = 10;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    EventManager eventManager;
    ItemManager itemManager;
    WeaponManager weaponManager;
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
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;
    SpawnerSystem spawnerSystem{*this};
    LevelUpSystem levelUpSystem;
    WeaponFireSystem weaponFireSystem;
    LevelUpHandler levelUpHandler{*this};
    UIRenderSystem uiRenderSystem;
    MouseInputSystem mouseInputSystem;
    PlayerStatListener playerStatListener{*this};
    PickUpSystem pickUpSystem;
    DebugRenderSystem debugRenderSystem{*this};
    HUDSystem hudSystem;
    PreRenderSystem preRenderSystem;


public:
    World();
    void update(float dt, SDL_Event& event, SceneType sceneType) {

        if (sceneType == SceneType::MainMenu) {
            //main menu system
            mainMenuSystem.update(event);
        } else if (isPaused) {
            keyboardInputSystem.update(*this, entities, event);
        } else {
            timer.startTimer("update");
            keyboardInputSystem.update(*this, entities, event);
            bobbingSystem.update(entities, dt);
            timer.startTimer("movement");
            movementSystem.update(entities, dt);
            timer.stopTimer("movement");
            enemyMovementSystem.update(entities, dt);
            spawnTimerSystem.update(entities, dt);
            timer.startTimer("pickUp");
            pickUpSystem.update(entities, *this);
            timer.stopTimer("pickUp");
            timer.startTimer("colliders");
            timer.startTimer("grid");
            gridSystem.update(entityGrid, entities, *this);
            timer.stopTimer("grid");
            timer.startTimer("collision");
            collisionSystem.update(*this, timer);
            timer.stopTimer("collision");
            timer.stopTimer("colliders");
            effectSystem.update(entities, dt);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            destructionSystem.update(entities);
            levelUpSystem.update(entities, *this);
            weaponFireSystem.update(*this, dt);
            hudSystem.update(entities);
            timer.stopTimer("update");
        }
        mouseInputSystem.update(*this, event);
        preRenderSystem.update(entities);


        synchronizeEntities();
        cleanup();
    }

    void render() {

        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                map.draw(e->getComponent<Camera>());
                if (debugState.debug && debugState.grid) {
                    gridSystem.draw(e->getComponent<Camera>());
                }
            }
        }

        timer.startTimer("render");
        renderSystem.render(entities);
        timer.stopTimer("render");
        if (debugState.debug) {
            debugRenderSystem.render(entities, debugState);
        }
        uiRenderSystem.render(entities);

        if (debugState.debug && debugState.timer && !isPaused) {
            timer.printResults();
            std::cout << "Entity#: " <<  entities.size() << "\n" << std::endl;
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

    ItemManager& getItemManager() {
        return itemManager;
    }
    WeaponManager& getWeaponManager() {
        return weaponManager;
    }


    Map& getMap() {return map;}

    DebugState& getDebugState() {return debugState;}

    Entity* getPlayer() {
        return player;
    }

    void setPlayer(Entity* player) {
        this->player = player;
    }

    void togglePaused() {
        isPaused = !isPaused;
    }

};

#endif //INC_8051TUTORIAL_WORLD_H