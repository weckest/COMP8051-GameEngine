//
// Created by BCIT on 2026-04-03.
//

#ifndef INC_8051TUTORIAL_EFFECTANIMATIONSYSTEM_H
#define INC_8051TUTORIAL_EFFECTANIMATIONSYSTEM_H
#include <memory>
#include <vector>

#include "AnimationClip.h"
#include "Component.h"
#include "Entity.h"

class EffectAnimationSystem {
	public:
		void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
			for (auto& entity : entities) {

				if (!entity->hasComponent<Animation>() ||
				    !entity->hasComponent<Sprite>() ||
				    !entity->hasComponent<Lifetime>())
					continue;

				auto& anim = entity->getComponent<Animation>();
				auto& sprite = entity->getComponent<Sprite>();
				auto& lifetime = entity->getComponent<Lifetime>();

				// no clip dont do
				if (anim.currentClip.empty()) continue;

				const auto& clip = anim.clips.at(anim.currentClip);

				anim.time += dt;

				if (anim.time >= anim.speed) {
					anim.time -= anim.speed;

					anim.currentFrame++;

					if (anim.currentFrame >= clip.frameIndices.size()) {
						anim.currentFrame = clip.frameIndices.size() - 1;
						lifetime.time = 0.0f;
					}
				}

				const auto& frame = clip.frameIndices[anim.currentFrame];

				sprite.src.x = frame.x;
				sprite.src.y = frame.y;
				sprite.src.w = frame.w;
				sprite.src.h = frame.h;
			}
		}
	};


#endif //INC_8051TUTORIAL_EFFECTANIMATIONSYSTEM_H