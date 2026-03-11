//
// Created by Weckest on 2026-03-10.
//

#ifndef INC_8051TUTORIAL_GRIDPOSITION_H
#define INC_8051TUTORIAL_GRIDPOSITION_H
#include "Vector2D.h"

class GridPosition {
public:
    Vector2D tl;
    Vector2D br;

    int getHeight() {
        return tl.y - br.y;
    }

    int getWidth() {
        return tl.x - br.x;
    }
};

#endif //INC_8051TUTORIAL_GRIDPOSITION_H