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

            float percent = (slider.value - slider.min) / (slider.max - slider.min);

            // Position knob along bar
            float knobX = collider.rect.x + percent * collider.rect.w;

            t.position.x = knobX - sprite.dst.w / 2.0f;
            t.position.y = collider.rect.y + (collider.rect.h - sprite.dst.h) / 2.0f;

            // Sync sprite position
            sprite.dst.x = t.position.x;
            sprite.dst.y = t.position.y;

            if (entity->hasComponent<Collider>())
            {
                auto& col = entity->getComponent<Collider>();
                col.rect.x = sprite.dst.x;
                col.rect.y = sprite.dst.y;
            }
        }
    }
};

#endif //INC_8051TUTORIAL_UISLIDERSYSTEM_H