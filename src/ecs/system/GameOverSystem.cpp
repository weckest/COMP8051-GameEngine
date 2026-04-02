//
// Created by Weckest on 2026-03-04.
//

#include "GameOverSystem.h"

#include "Game.h"
#include "SDL3/SDL_events.h"

void GameOverSystem::update(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                Game::onSceneChangeRequest("mainmenu");
                break;
            default:
                break;
        }
    }
}
