//
// Created by BCIT on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H
#define INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H
#include <SDL3/SDL_events.h>

class World;

class MouseInputSystem {
public:
    void update(World& world, const SDL_Event& e);
};

#endif //INC_8051TUTORIAL_MOUSEINPUTSYSTEM_H