#include <iostream>

#include "Game.h"

//global variable
Game* game = nullptr;

void test() {
    Vector2D a(3, 4);
    Vector2D b(1, 2);

    a += b;
    Vector2D c = a - b;
    Vector2D d = a * 2;
    Vector2D e = 2 * a;
    Vector2D f = -a;
    bool same = (a==b);
    a *= 2;
    b /= 2;
    e -= a;
    bool notSame = (a!=b);
}

int main() {

    const int FPS = 60; //60 is the closest refresh rate of most our monitors,  30fps is the half the work
    const int desiredFrameTime = 1000 / FPS; // 16ms per frame

    Uint64 ticks = SDL_GetTicks();
    int actualFrameTime;

    game = new Game();
    game->init("8051 Tutorial Engine", 800, 600, false);

    //game loop
    while (game->running()) {

        float deltaTime = (SDL_GetTicks() - ticks) / 1000.0f;

        ticks = SDL_GetTicks(); //time in milliseconds since we initialzed SDL

        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks; //elapsed time in ms it took the current frame

        if (actualFrameTime != 0) {
            SDL_SetWindowTitle(game->getWindow(),
                ("8051 Tutorial Engine, FPS: " + std::to_string((int)(1.0f / deltaTime))
                    + ", Actual: " + std::to_string((int)(1.0f / (actualFrameTime / 1000.f)))).c_str());
        }
        // std::cout << actualFrameTime << " " << deltaTime << std::endl;
        //frame limiter
        //keeps the game running at the desirer frame rate
        //if the actual frame took less time that the desired frame, delay the difference
        if (desiredFrameTime > actualFrameTime) {
            SDL_Delay(desiredFrameTime - actualFrameTime);
        }
    }

    delete game;

    return 0;
}