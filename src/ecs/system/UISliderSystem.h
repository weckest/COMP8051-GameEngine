//
// Created by Miguel on 2026-04-04.
//

#ifndef INC_8051TUTORIAL_UISLIDERSYSTEM_H
#define INC_8051TUTORIAL_UISLIDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class UISliderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity>>& entities)
    {
        for (auto& entity : entities)
        {
            if (!entity->hasComponent<SliderKnob>() ||
                !entity->hasComponent<Transform>() ||
                !entity->hasComponent<Sprite>()) continue;

            auto& knob = entity->getComponent<SliderKnob>();
            auto* sliderEntity = knob.slider;

            if (!sliderEntity || !sliderEntity->hasComponent<Slider>() ||
                !sliderEntity->hasComponent<Collider>()) continue;

            auto& slider = sliderEntity->getComponent<Slider>();
            auto& collider = sliderEntity->getComponent<Collider>();

            auto& t = entity->getComponent<Transform>();
            auto& sprite = entity->getComponent<Sprite>();

            float knobHalf = sprite.dst.w / 2.0f;
            float minX = collider.rect.x + knobHalf;
            float maxX = collider.rect.x + collider.rect.w - knobHalf;

            float percent = (slider.value - slider.min) / (slider.max - slider.min);
            float knobX = minX + percent * (maxX - minX);

            t.position.x = knobX - knobHalf;
            t.position.y = collider.rect.y + (collider.rect.h - sprite.dst.h) / 2.0f;

            sprite.dst.x = t.position.x;
            sprite.dst.y = t.position.y;

            if (entity->hasComponent<Collider>()) {
                auto& col = entity->getComponent<Collider>();
                col.rect.x = t.position.x;
                col.rect.y = t.position.y;
            }
        }
    }
};

#endif //INC_8051TUTORIAL_UISLIDERSYSTEM_H