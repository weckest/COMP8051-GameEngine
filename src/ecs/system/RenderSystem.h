//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_RENDERSYSTEM_H
#define INC_8051TUTORIAL_RENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "TextureManager.h"
#include "SDL3/SDL_render.h"

class RenderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity>>& entities) {

        Entity* cameraEntity = nullptr;

        //Find camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) return; //no camera
        auto& cam = cameraEntity->getComponent<Camera>();

        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                auto& t = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();

                //we are converting from world space to screen space
                sprite.dst.x = t.position.x - cam.view.x;
                sprite.dst.y = t.position.y - cam.view.y;

                //if the entity has that animation component, update the src rect
                if (entity->hasComponent<Animation>()) {
                    auto& anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
                }


                TextureManager::draw(sprite.texture, sprite.src, sprite.dst);

                //collider debugs
                // if (entity->hasComponent<Collider>()) {
                //     auto& collider = entity->getComponent<Collider>();
                //     std::cout << collider.tag << " " << collider.rect.x << " " << collider.rect.y << " " << collider.rect.w << " " << collider.rect.h << std::endl;
                //     SDL_Texture* tex = TextureManager::load("../assets/spritesheet.png");
                //     SDL_FRect src {0,32,32,32};
                //     SDL_FRect dst {collider.rect.x - cam.view.x,collider.rect.y - cam.view.y,collider.rect.w,collider.rect.h};
                //
                //     TextureManager::draw(tex, src, dst);
                // }
            }
        }
    }
};

#endif //INC_8051TUTORIAL_RENDERSYSTEM_H