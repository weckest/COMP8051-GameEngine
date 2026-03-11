//
// Created by Weckest on 2026-03-05.
//

#include "KeyboardInputSystem.h"
#include "World.h"

void KeyBoardInputSystem::update(World& world, const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
            world.getEventManager().emit(ToggleDebugEvent{});
        } else {
            world.getEventManager().emit(KeyPressEvent{event.key.key});
        }
    }

    for (auto& e : entities) {
        if (e->hasComponent<Velocity>() && e->hasComponent<PlayerTag>()) {
            auto& v = e->getComponent<Velocity>();
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_W:
                        v.direction.y = -1;
                        break;
                    case SDLK_A:
                        v.direction.x = -1;
                        break;
                    case SDLK_S:
                        v.direction.y = 1;
                        break;
                    case SDLK_D:
                        v.direction.x = 1;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_EVENT_KEY_UP) {
                switch (event.key.key) {
                    case SDLK_W:
                        v.direction.y = 0;
                        break;
                    case SDLK_A:
                        v.direction.x = 0;
                        break;
                    case SDLK_S:
                        v.direction.y = 0;
                        break;
                    case SDLK_D:
                        v.direction.x = 0;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
