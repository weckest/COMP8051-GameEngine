//
// Created by Weckest on 2026-02-18.
//

#ifndef INC_8051TUTORIAL_ANIMATIONCLIP_H
#define INC_8051TUTORIAL_ANIMATIONCLIP_H
#include <vector>

#include "SDL3/SDL_rect.h"

struct AnimationClip {
    std::vector<SDL_FRect> frameIndices;
};

#endif //INC_8051TUTORIAL_ANIMATIONCLIP_H