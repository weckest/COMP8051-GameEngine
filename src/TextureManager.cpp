//
// Created by Weckest on 2026-01-14.
//

#include "TextureManager.h"
#include "Game.h"

#include <iostream>

//doesnt create any extra memory justs grabs the reference of this object
extern Game* game;
std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;

//load our texture with a path
SDL_Texture* TextureManager::load(const char* path) {

    //Caching: storing the result of some work so you dont have to repeat it next time
    //Check if the texture already exists in the map
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }

    //a surface represents an image in ram (cpu memory)
    //do we want to do anything with the image before we store it in gpu?
    SDL_Surface* tempSurface = IMG_Load(path);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << path << std::endl;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);

    //clean up surface
    SDL_DestroySurface(tempSurface);

    //check if the texture creation was successful
    if (!texture) {
        std::cout << "Failed to create texture: " << path << std::endl;
        return nullptr;
    }

    //store the new texture in the cache
    textures[path] = texture;


    return texture;
}

void TextureManager::draw(SDL_Texture *texture, SDL_FRect src, SDL_FRect dst) {
    //draw the texture to the screen form vram
    SDL_RenderTexture(game->renderer, texture, &src, &dst);
}

void TextureManager::drawLine(Vector2D start, Vector2D end) {
    SDL_RenderLine(game->renderer, start.x, start.y, end.x, end.y);
}

void TextureManager::drawLine(Vector2D start, Vector2D end, Uint8 r, Uint8 g, Uint8 b) {
    Uint8 oldR, oldG, oldB, oldA;
    SDL_GetRenderDrawColor(game->renderer, &oldR, &oldG, &oldB, &oldA);
    SDL_SetRenderDrawColor(game->renderer, r, g, b, 255);
    drawLine(start, end);
    SDL_SetRenderDrawColor(game->renderer, oldR, oldG, oldB, oldA);
}

void TextureManager::drawCircle(Vector2D& center, float radius, Uint8 r, Uint8 g, Uint8 b) {
    Uint8 oldR, oldG, oldB, oldA;
    float buffer = 1.0f;
    float thickness = 5.0f;
    float stepSize = 1.0f;
    std::vector<SDL_FPoint> points;
    points.resize(points.size() + 1);
    SDL_GetRenderDrawColor(game->renderer, &oldR, &oldG, &oldB, &oldA);
    SDL_SetRenderDrawColor(game->renderer, r, g, b, 255);
    float inner = radius - 5 + buffer;
    float outer = radius - 5 + thickness;

    float innerSq = inner * inner;
    float outerSq = outer * outer;
    for (float i = center.x - radius; i <= center.x + radius; i += stepSize) {
        float dx = i - center.x;

        float maxY = sqrt(radius * radius - dx * dx);

        for (float j = center.y - maxY; j <= center.y + maxY; j += stepSize) {
            float dy = j - center.y;
            float distSq = dx * dx + dy * dy;

            if (distSq >= innerSq && distSq <= outerSq) {
                points.push_back({i, j});
            }
        }
    }

    SDL_RenderPoints(game->renderer, points.data(), points.size());
    SDL_SetRenderDrawColor(game->renderer, oldR, oldG, oldB, oldA);
}


void TextureManager::clean() {
    for (auto& texture : textures) {
        SDL_DestroyTexture(texture.second);
        texture.second = nullptr;
    }
    textures.clear();
}
