//
// Created by BCIT on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_UIRENDERSYSTEM_H
#define INC_8051TUTORIAL_UIRENDERSYSTEM_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "Component.h"
#include "RenderUtils.h"
#include "../../manager/TextureManager.h"

class UIRenderSystem {
public:

    void render(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* camera = nullptr;

        // Find camera (only needed for debug labels)
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                camera = e.get();
                break;
            }
        }

        if (!camera) return;
        auto& cam = camera->getComponent<Camera>();

        for (auto& e : entities) {

            if (!e->hasComponent<Transform>()) continue;
            auto& transform = e->getComponent<Transform>();

            //
            // ===== SPRITES (UI) =====
            //
            if (e->hasComponent<Sprite>()) {
                auto& sprite = e->getComponent<Sprite>();

                if (sprite.renderLayer != RenderLayer::UI) continue;
                if (!sprite.visible) continue;

                // COPY, do NOT modify original
                SDL_FRect dst = sprite.dst;

                // Apply transform position (UI = no camera)
                dst.x = transform.position.x;
                dst.y = transform.position.y;

                // Apply scale
                SDL_FRect scaledDest = RenderUtils::getScaledDest(dst, transform.scale);

                // Draw
                TextureManager::draw(sprite.texture, &sprite.src, &scaledDest);
            }

            //
            // ===== LABELS =====
            //
            else if (e->hasComponent<Label>()) {
                auto& label = e->getComponent<Label>();

                if (!label.visible) continue;

                // COPY, do NOT modify original
                SDL_FRect dst = label.dst;

                // Apply transform
                dst.x = transform.position.x;
                dst.y = transform.position.y;

                // Only debug labels follow camera
                if (label.type == LabelType::Debug) {
                    dst.x -= cam.view.x;
                    dst.y -= cam.view.y;
                }

                // Apply scale
                SDL_FRect scaledDest = RenderUtils::getScaledDest(dst, transform.scale);

                // Draw
                TextureManager::draw(label.texture, nullptr, &scaledDest);
            }
        }
    }
};

#endif //INC_8051TUTORIAL_UIRENDERSYSTEM_H