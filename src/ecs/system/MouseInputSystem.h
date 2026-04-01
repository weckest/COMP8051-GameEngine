//
// Created by BCIT on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H
#define INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

class World;

class MouseInputSystem {
public:
    void update(World& world, const SDL_Event& e, SDL_Renderer* renderer);
};

#endif //INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H