//
// Created by Weckest on 2026-01-14.
//

#ifndef INC_8051TUTORIAL_GAMEOBJECT_H
#define INC_8051TUTORIAL_GAMEOBJECT_H

#include "Game.h"

class GameObject {
public:
    GameObject(const char* path, float x, float y);
    ~GameObject();

    void update(float deltaTime);
    void draw();
private:
    float xPos{}, yPos{};

    SDL_Texture *texture = nullptr;
    SDL_FRect srcRect{}, dstRect{};
};

#endif //INC_8051TUTORIAL_GAMEOBJECT_H