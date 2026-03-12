//
// Created by BCIT on 2026-03-11.
//

#include "MouseInputSystem.h"

#include "World.h"

void MouseInputSystem::update(World& world, const SDL_Event& e) {
	if (e.type != SDL_EVENT_MOUSE_MOTION &&
		e.type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
		e.type != SDL_EVENT_MOUSE_BUTTON_UP) {
		return;
		}

	float mx, my;
	SDL_GetMouseState(&mx, &my);

	for (auto& entity : world.getEntities()) {
		if (entity->hasComponent<Clickable>() && entity->hasComponent<Collider>()) {
			Clickable& clickable = entity->getComponent<Clickable>();
			Collider& collider = entity->getComponent<Collider>();

			if (!collider.enabled) continue;

			bool inside = (mx >= collider.rect.x  && mx <= collider.rect.x + collider.rect.w &&
				my >= collider.rect.y && my <= collider.rect.y + collider.rect.h);

			//hover

			if (e.type == SDL_EVENT_MOUSE_MOTION) {
				if (!inside && clickable.pressed) {
					//cancel event
					world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Cancel });
				}
			}

			if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					if (inside) {
						clickable.pressed = true;
						//pressed event
						world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Pressed });
					}
				}
			}

			if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					if (inside) {
						clickable.pressed = false;
						//released event
						world.getEventManager().emit(MouseInteractionEvent{entity.get(), MouseInteractionState::Released });

					}
				}
			}
		}
	}
}

