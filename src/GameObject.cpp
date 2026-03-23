//
// Created by Weckest on 2026-01-14.
//

#include "GameObject.h"

#include <iostream>

#include "manager/TextureManager.h"


GameObject::GameObject(const char *path, float x, float y) :
xPos(x), yPos(y), texture(TextureManager::load(path)) {

}

GameObject::~GameObject() {
    if (texture) SDL_DestroyTexture(texture);
}

void GameObject::update(float deltaTime) {
    //move game object one pixel on x and y axis each frame
    xPos += 100.0f * deltaTime;
    yPos += 100.0f * deltaTime;

    //where do we want to crop texture from?
    srcRect.x = srcRect.y = 0;

    //how much to crop?
    srcRect.w = srcRect.h = 32;

    //where to draw it?
    dstRect.x = xPos;
    dstRect.y = yPos;

    //what sizes to render?
    dstRect.w = srcRect.w;
    dstRect.h = srcRect.h;
}

void GameObject::draw() {
    TextureManager::draw(texture, &srcRect, &dstRect);
}
