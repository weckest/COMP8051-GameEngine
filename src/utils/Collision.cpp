//
// Created by Weckest on 2026-02-04.
//

#include "Collision.h"

#include "SDL3/SDL_rect.h"

//axis aligned bounding box
//passing in rects
bool Collision::AABB(const SDL_FRect& rectA, const SDL_FRect& rectB) {

    //is the right edge of rect A >= to the left edge of rectB
    //is the right edge of rect B >= to the left edge of rectA
    //is the bottom edge of RectA >= to the top of RectB
    //is the bottom edge of rectB >= to the top of rectA

    if (
        rectA.x + rectA.w >= rectB.x &&
        rectB.x + rectB.w >= rectA.x &&
        rectA.y + rectA.h >= rectB.y &&
        rectB.y + rectB.h >= rectA.y) {
        return true;
    }
    return false;
}

//passing in the colliders itself
bool Collision::AABB(const Collider &colA, const Collider &colB) {
    return AABB(colA.rect, colB.rect);
}
