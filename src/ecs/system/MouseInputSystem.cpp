//
// Created by BCIT on 2026-03-11.
//

#include "MouseInputSystem.h"

#include "Game.h"
#include "World.h"

void MouseInputSystem::update(World& world, const SDL_Event& e, SDL_Renderer* renderer) {
	if (e.type != SDL_EVENT_MOUSE_MOTION &&
		e.type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
		e.type != SDL_EVENT_MOUSE_BUTTON_UP) {
		return;
		}

	float mx, my;
	SDL_GetMouseState(&mx, &my);

	float logicalX, logicalY;
	SDL_RenderCoordinatesFromWindow(renderer, mx, my, &logicalX, &logicalY);

	for (auto& entity : world.getEntities()) {
		if (entity->hasComponent<Clickable>() && entity->hasComponent<Collider>()) {
			Clickable& clickable = entity->getComponent<Clickable>();
			Collider& collider = entity->getComponent<Collider>();

			if (!collider.enabled) continue;


			bool inside = (logicalX >= collider.rect.x &&
		   logicalX <= collider.rect.x + collider.rect.w &&
		   logicalY >= collider.rect.y &&
		   logicalY <= collider.rect.y + collider.rect.h);

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

