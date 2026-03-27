//
// Created by Weckest on 2026-02-18.
//

#ifndef INC_8051TUTORIAL_CAMERASYSTEM_H
#define INC_8051TUTORIAL_CAMERASYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class CameraSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        //find the player
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) return;

        auto& playerTransform = playerEntity->getComponent<Transform>();
        auto& playerSprite = playerEntity->getComponent<Sprite>();
        for (auto& e: entities) {
            if (e->hasComponent<Camera>()) {
                auto& cam = e->getComponent<Camera>();

                //positions the camera so the player is at the center of its view
                cam.view.x = playerTransform.position.x - cam.view.w / 2 + (playerSprite.dst.w / 2);
                cam.view.y = playerTransform.position.y - cam.view.h / 2 + (playerSprite.dst.h / 2);

                //clamp camera
                //the camera is positioning its self to the player is center but the player could walk off the screen
                //so clamp the camera so it stays within the window
                if (cam.view.x < 0) {
                    cam.view.x = 0;
                }
                if (cam.view.y < 0) {
                    cam.view.y = 0;
                }

                if (cam.view.x > cam.worldWidth - cam.view.w) {
                    cam.view.x = cam.worldWidth - cam.view.w;
                }
                if (cam.view.y > cam.worldHeight - cam.view.h) {
                    cam.view.y = cam.worldHeight - cam.view.h;
                }
            }
        }
    }
};

#endif //INC_8051TUTORIAL_CAMERASYSTEM_H