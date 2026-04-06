//
// Created by migue on 2026-04-01.
//

#include "OverlayRenderSystem.h"

#include <cmath>
#include <utility>

#include "manager/TextureManager.h"

void OverlayRenderSystem::receiveData(std::vector<std::vector<int>> data, int mapWidth, int mapHeight, int mapScale, SDL_Texture *mapTileset) {
    overlayData = std::move(data);
    width = mapWidth;
    height = mapHeight;
    scale = mapScale;
    tileset = mapTileset;
    hasData = true;
}

void OverlayRenderSystem::draw(const Camera& cam) {
    SDL_FRect src{}, dest{};

    dest.w = dest.h = scale;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = overlayData[row][col];
            if (type == 0) continue;

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            //move the tiles or map relative to the camera
            //convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            int tileX = (type-1) % 20; // -1 because TMX start from 1 but pixels are 0
            int tileY = type / 20;
            if (tileX == 19) { //handle tileX -1
                tileY -= 1;
            }

            src.x = tileX * scale;
            src.y = tileY * scale;
            src.w = scale;
            src.h = scale;

            TextureManager::draw(tileset, &src, &dest);
        }
    }
}
