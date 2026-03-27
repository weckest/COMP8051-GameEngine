//
// Created by Weckest on 2026-03-05.
//

#include "RenderSystem.h"
#include "World.h"

void RenderSystem::render(const std::vector<std::unique_ptr<Entity>>& entities) {
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

            if (sprite.renderLayer != RenderLayer::World) continue;
            //
            // //SCALING
            if (t.scale != 1.0f)
            {
                if (!sprite.hasBase)
                {
                    sprite.baseDst = sprite.dst;
                    sprite.hasBase = true;
                }

                sprite.dst.w = sprite.baseDst.w * t.scale;
                sprite.dst.h = sprite.baseDst.h * t.scale;

                //Scale collider to match the sprite size
                if (entity->hasComponent<Collider>())
                {
                    auto& c = entity->getComponent<Collider>();

                    if (!c.hasBase)
                    {
                        c.baseRect = c.rect;
                        c.hasBase = true;
                    }
                    c.rect.w = c.baseRect.w * t.scale;
                    c.rect.h = c.baseRect.h * t.scale;
                }
            }

            //we are converting from world space to screen space
            sprite.dst.x = t.position.x - cam.view.x;
            sprite.dst.y = t.position.y - cam.view.y;

            //if the entity has that animation component, update the src rect
            if (entity->hasComponent<Animation>()) {
                auto& anim = entity->getComponent<Animation>();
                sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
            }

            //ROTATION
            if (t.rotation != 0.0f)
            {
                SDL_FPoint center;
                center.x = sprite.dst.w / 2.0f;
                center.y = sprite.dst.h / 2.0f;

                TextureManager::draw(sprite.texture, &sprite.src, &sprite.dst, t.rotation, &center);
            } else
            {
                TextureManager::draw(sprite.texture, &sprite.src, &sprite.dst);
            }


        }
    }
}
