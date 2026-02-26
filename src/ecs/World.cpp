//
// Created by Weckest on 2026-02-04.
//

#include "World.h"

#include <iostream>

#include "Game.h"

void callback(const CollisionEvent& collision) {
    if (collision.entityA == nullptr || collision.entityB == nullptr) return;
    if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;

    auto& colliderA = collision.entityA->getComponent<Collider>();
    auto& colliderB = collision.entityB->getComponent<Collider>();

    std::cout << "A collision occurred between entity " << colliderA.tag << " and Entity B " << colliderB.tag << std::endl;

}

World::World() {
    //subscribe to the collision events
    eventManager.subscribe([this](const CollisionEvent& collision) {

        Entity* sceneStateEntity = nullptr;

        //find scene state
        for (auto& e : entities) {
            if (e->hasComponent<SceneState>()) {
                sceneStateEntity = e.get();
                break;
            }
        }

        if (collision.entityA == nullptr || collision.entityB == nullptr) return;

        if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;

        auto& colliderA = collision.entityA->getComponent<Collider>();
        auto& colliderB = collision.entityB->getComponent<Collider>();

        Entity* player = nullptr;
        Entity* item = nullptr;
        Entity* wall = nullptr;
        Entity* enemy = nullptr;
        Entity* bullet = nullptr;

        if (colliderA.tag == "player" && colliderB.tag == "item") {
            player = collision.entityA;
            item = collision.entityB;
        } else if (colliderA.tag == "item" && colliderB.tag == "player") {
            player = collision.entityB;
            item = collision.entityA;
        }

        if (player && item) {
            item->destroy();
            //scene state
            if (sceneStateEntity) {
                auto& sceneState = sceneStateEntity->getComponent<SceneState>();
                sceneState.coinsCollected++;
                if (sceneState.coinsCollected > 1) {
                    Game::onSceneChangeRequest("level2");
                }
            }
        }

        player = nullptr;
        item = nullptr;

        //player vs wall
        if (colliderA.tag == "player" && colliderB.tag == "wall") {
            player = collision.entityA;
            wall = collision.entityB;
        } else if (colliderA.tag == "wall" && colliderB.tag == "player") {
            player = collision.entityB;
            wall = collision.entityA;
        }

        if (player && wall) {
            //stop the player
            auto& t = player->getComponent<Transform>();
            t.position = t.oldPosition;
        }

        player = nullptr;
        wall = nullptr;

        //player vs enemy
        if (colliderA.tag == "player" && colliderB.tag == "enemy") {
            player = collision.entityA;
            enemy = collision.entityB;
        } else if (colliderA.tag == "enemy" && colliderB.tag == "player") {
            player = collision.entityB;
            enemy = collision.entityA;
        }

        if (player && enemy) {
            player->destroy();
            //change scenes defer
            Game::onSceneChangeRequest("gameover");
        }

        player = nullptr;
        enemy = nullptr;

        //bullet vs enemy
        // std::cout << colliderA.tag << " against " << colliderB.tag << std::endl;
        if (colliderA.tag == "bullet" && colliderB.tag == "enemy") {
            bullet = collision.entityA;
            enemy = collision.entityB;
        } else if (colliderA.tag == "enemy" && colliderB.tag == "bullet") {
            bullet = collision.entityB;
            enemy = collision.entityA;
        }

        if (bullet && enemy) {
            bullet->destroy();
            enemy->destroy();
        }

        bullet = nullptr;
        wall = nullptr;
    });

    // eventManager.subscribe(callback);
}



