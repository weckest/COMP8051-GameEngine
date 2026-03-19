//
// Created by Weckest on 2026-03-18.
//

#include "DebugRenderSystem.h"
#include "World.h"

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>> &entities, DebugState debugState) {
    Entity* player = world.getPlayer();
    Entity* cameraEntity = nullptr;


    for (auto& e : entities) {
        if (e->hasComponent<Camera>()) {
            cameraEntity = e.get();
            break;
        }
    }

    if (!player) return;
    if (!cameraEntity) return;
    auto& cam = cameraEntity->getComponent<Camera>();

    auto& playerSprite = player->getComponent<Sprite>();
    auto& playerTransform = player->getComponent<Transform>();
    auto& stats = player->getComponent<Stats>();

    Vector2D playerCenter = playerTransform.position + Vector2D{playerSprite.dst.w / 2, playerSprite.dst.h / 2};
    //move into view
    playerCenter.x -= cam.view.x;
    playerCenter.y -= cam.view.y;


    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
            auto& t = e->getComponent<Transform>();
            auto& sprite = e->getComponent<Sprite>();

            //collider debugs
            //only show if debug mode is on
            if (e->hasComponent<Collider>()) {
                auto& collider = e->getComponent<Collider>();
                // std::cout << collider.tag << " " << collider.rect.x << " " << collider.rect.y << " " << collider.rect.w << " " << collider.rect.h << std::endl;
                SDL_Texture* tex = TextureManager::load("../assets/spritesheet.png");
                SDL_FRect src {0,32,32,32};
                SDL_FRect dst {collider.rect.x - cam.view.x,collider.rect.y - cam.view.y,collider.rect.w,collider.rect.h};

                TextureManager::draw(tex, src, dst);
            }
        }
    }

    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
            auto& t = e->getComponent<Transform>();
            auto& sprite = e->getComponent<Sprite>();

            if (e->hasComponent<ItemTag>()) {
                Vector2D aCenter = t.position + Vector2D{(sprite.dst.w / 2), (sprite.dst.h / 2)};
                float aoe = stats.aoeModifier * 64;
                float change = aoe / (playerCenter - aCenter).length();
                std::cout << "Player: " << playerCenter.x << ' ' << playerCenter.y << std::endl;
                std::cout << "Entity: " << aCenter.x << ' ' << aCenter.y << std::endl;
                std::cout << "Change: " << change << std::endl;
                aCenter.x -= cam.view.x;
                aCenter.y -= cam.view.y;


                TextureManager::drawLine(aCenter, playerCenter);
                TextureManager::drawLine(playerCenter - (playerCenter - aCenter) * change, playerCenter, 0, 127, 255);


            }
        }
    }
}
