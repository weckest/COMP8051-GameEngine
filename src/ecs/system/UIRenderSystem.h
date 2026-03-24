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
		for (auto& e: entities) {
			if (e->hasComponent<Camera>()) {
				camera = e.get();
			}
		}
		if (!camera) return;
		auto& cam = camera->getComponent<Camera>();

		for (auto& e : entities) {
			if (e->hasComponent<Transform>()) {
				auto& transform = e->getComponent<Transform>();

				if (e->hasComponent<Sprite>()) {
					auto& sprite = e->getComponent<Sprite>();

					if (sprite.renderLayer != RenderLayer::UI) continue;

					// no converting from world space to screen space
					sprite.dst.x = transform.position.x;
					sprite.dst.y = transform.position.y;

					if (sprite.visible) {
						SDL_FRect scaledDest = RenderUtils::getScaledDest(sprite.dst, transform.scale);
						TextureManager::draw(sprite.texture, &sprite.src, &scaledDest);
					}
				} else if (e->hasComponent<Label>()) {
					auto& label = e->getComponent<Label>();

					label.dst.x = transform.position.x;
					label.dst.y = transform.position.y;

					if (label.type == LabelType::Debug) {
						label.dst.x -= cam.view.x;
						label.dst.y -= cam.view.y;
					}

					if (label.visible) {
						SDL_FRect scaledDest = RenderUtils::getScaledDest(label.dst, transform.scale);
						TextureManager::draw(label.texture, nullptr, &scaledDest);
					}
				}
			}
		}
	}
};

#endif //INC_8051TUTORIAL_UIRENDERSYSTEM_H