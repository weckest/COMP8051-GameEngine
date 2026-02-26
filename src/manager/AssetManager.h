//
// Created by Weckest on 2026-02-18.
//

#ifndef INC_8051TUTORIAL_ASSETMANAGER_H
#define INC_8051TUTORIAL_ASSETMANAGER_H
#include <unordered_map>

#include "Component.h"

class AssetManager {
    static std::unordered_map<std::string, Animation> animations;
    static Animation loadAnimationFromXML(const char* path);

public:
    static void loadAnimation(const std::string& clipName, const char* path);
    static const Animation& getAnimation(const std::string& clipName);
};

#endif //INC_8051TUTORIAL_ASSETMANAGER_H