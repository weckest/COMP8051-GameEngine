//
// Created by Weckest on 2026-01-14.
//

#include "Map.h"

#include <chrono>
#include <cmath>
#include <iostream>

#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>


void Map::load(const char *path, SDL_Texture *ts) {

    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    //parse width and height of map
    auto* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    //parse terrain data
    auto* layer = mapNode->FirstChildElement("layer");
    auto* data = layer->FirstChildElement("data");
    std::string csv = data->GetText();

    std::stringstream ss(csv);
    tileData = std::vector(height, std::vector<int>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::string val;
            //read chars from ss into val until it hits a comma,, or is at the end of the stream

            if (!std::getline(ss, val, ',')) break;
            tileData[i][j] = std::stoi(val); //string to int converter


        }
    }

    //parse building layer data
    if (auto* buildingLayer = layer->NextSiblingElement("layer"); buildingLayer != nullptr) {
        auto* buildingData = buildingLayer->FirstChildElement("data");
        std::string buildingCsv = buildingData->GetText();

        std::stringstream building_ss(buildingCsv);
        buildingTileData = std::vector(height, std::vector<int>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                std::string building_val;
                //read chars from ss into val until it hits a comma,, or is at the end of the stream

                if (!std::getline(building_ss, building_val, ',')) break;
                buildingTileData[i][j] = std::stoi(building_val); //string to int converter
            }
        }
    }

    //parse collider data
    auto* objectGroup = layer->NextSiblingElement("objectgroup");
    while (objectGroup != nullptr) {
        if (objectGroup->Attribute("name", "Collision Layer") != nullptr) {
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = obj->FloatAttribute("width");
                c.rect.h = obj->FloatAttribute("height");
                colliders.push_back(c);
                }
        }

        //parse item data
        if (objectGroup->Attribute("name", "Item Layer") != nullptr) {
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = 32;
                c.rect.h = 32;
                items.push_back(c);
                }
        }

        if (objectGroup->Attribute("name", "Spawner Layer") != nullptr) {
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                Transform t;
                t.position.x = obj->FloatAttribute("x");
                t.position.y = obj->FloatAttribute("y");
                spawners.push_back(t);
            }
        }

        objectGroup = objectGroup->NextSiblingElement("objectgroup");
    }

}

void Map::draw(const Camera& cam) {

    SDL_FRect src{}, dest{};

    dest.w = dest.h = scale;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            //move the tiles or map relative to the camera
            //convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);


            //FOR THE GAME
            int tileX = (type-1) % 20; // -1 because TMX start from 1 but pixels are 0
            // if (tileX == 0) {
            //     tileX = 20;
            // }
            int tileY = type / 20;
            if (tileX == 19) { //handle tileX -1
                tileY -= 1;
            }

            src.x = tileX * scale;
            src.y = tileY * scale;
            src.w = scale;
            src.h = scale;


            //FOR THE LECTURE SLIDES
            // switch (type) {
            //     case 1:
            //         //dirt
            //         src.x = 0;
            //         src.y = 0;
            //         src.w = 32;
            //         src.h = 32;
            //         break;
            //     case 2:
            //         //grass
            //         src.x = 32;
            //         src.y = 0;
            //         src.w = 32;
            //         src.h = 32;
            //         break;
            //     case 4:
            //         //water
            //         src.x = 32;
            //         src.y = 32;
            //         src.w = 32;
            //         src.h = 32;
            //         break;
            //     default:
            //         break;
            // }

            TextureManager::draw(tileset, src, dest);
        }
    }

    //comment out if using lecture slides
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = buildingTileData[row][col];
            if (type == 0) continue;

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            //move the tiles or map relative to the camera
            //convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);


            //FOR THE GAME
            int tileX = (type-1) % 20; // -1 because TMX start from 1 but pixels are 0
            // if (tileX == 0) {
            //     tileX = 20;
            // }
            int tileY = type / 20;
            if (tileX == 19) { //handle tileX -1
                tileY -= 1;
            }

            src.x = tileX * scale;
            src.y = tileY * scale;
            src.w = scale;
            src.h = scale;

            TextureManager::draw(tileset, src, dest);
        }
    }
}