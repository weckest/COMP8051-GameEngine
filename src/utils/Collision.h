//
// Created by Weckest on 2026-02-04.
//

#ifndef INC_8051TUTORIAL_COLLISION_H
#define INC_8051TUTORIAL_COLLISION_H
#include "Component.h"
#include "SDL3/SDL_rect.h"

class Collision {
public:
    static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
    static bool AABB(const Collider& colA, const Collider& colB);
};

#endif //INC_8051TUTORIAL_COLLISION_H