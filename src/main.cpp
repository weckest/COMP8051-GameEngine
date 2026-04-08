#include <iostream>

#include "Game.h"

//global variable
Game* game = nullptr;

int main() {

    const int FPS = 60;
    const Uint64 desiredFrameTime = 1000000000ULL / FPS; // nanoseconds per frame

    Uint64 ticks = SDL_GetTicksNS();
    Uint64 actualFrameTime;

    game = new Game();
    game->init("The Final Corpse", 800, 600, false);

    //game loop
    while (game->running()) {

        Uint64 now = SDL_GetTicksNS();
        float deltaTime = (now - ticks) / 1000000000.0f; // convert ns → seconds
        ticks = now;

        game->handleEvents();

        if (game->isFullscreen) {
            SDL_SetWindowFullscreen(game->getWindow(), SDL_WINDOW_FULLSCREEN);
        } else {
            SDL_SetWindowFullscreen(game->getWindow(), 0); // windowed
        }

        game->update(deltaTime);
        game->render(deltaTime, actualFrameTime);

        actualFrameTime = SDL_GetTicksNS() - now; // frame time in ns

        // frame limiter
        if (desiredFrameTime > actualFrameTime) {
            SDL_Delay((desiredFrameTime - actualFrameTime) / 1000000); // ns → ms
        }
    }

    delete game;

    return 0;
}