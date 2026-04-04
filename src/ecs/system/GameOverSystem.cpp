//
// Created by Weckest on 2026-03-04.
//

#include "GameOverSystem.h"

#include "Game.h"
#include "SDL3/SDL_events.h"

void GameOverSystem::update(World& world, const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
                Game::onSceneChangeRequest("mainmenu");
                break;
            default:
                break;
        }
    }
}
