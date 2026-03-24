//
// Created by beckh on 2026-03-09.
//

#include "ItemManager.hpp"

#include <iostream>
#include <ostream>

#include "tinyxml2.h"

 std::unordered_map<std::string,Item > ItemManager::items;


const Item &ItemManager::getRandItem() {
    int index = rand() % items.size();
    auto it = items.begin();
    std::advance(it, index);
    return it->second;
}


void ItemManager::loadAllItems(const char* path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path) != tinyxml2::XML_SUCCESS) return;

    auto* root = doc.FirstChildElement("items");
    if (!root) return;

    for (auto* elem = root->FirstChildElement("item"); elem != nullptr; elem = elem->NextSiblingElement("item")) {
        Item item;
        if (auto* nameAttr = elem->Attribute("name")) {
            item.name = nameAttr;
        }

        if (auto* itemPath = elem->Attribute("path")) {
            //std::cout << itemPath << std::endl;
            item.path = itemPath;
        }

        //check to make sure value exists before adding
        float value;
        if (elem->QueryFloatAttribute("damageModifier", &value) == tinyxml2::XML_SUCCESS)
            item.damageModifier = value;
        if (elem->QueryFloatAttribute("speedModifier", &value) == tinyxml2::XML_SUCCESS)
            item.speedModifier = value;
        if (elem->QueryFloatAttribute("fireRateModifier", &value) == tinyxml2::XML_SUCCESS)
            item.fireRateModifier = value;
        if (elem->QueryFloatAttribute("playerSizeModifier", &value) == tinyxml2::XML_SUCCESS)
            item.playerSizeModifier = value;
        if (elem->QueryFloatAttribute("projectileSizeModifier", &value) == tinyxml2::XML_SUCCESS)
            item.projectileSizeModifier = value;
        if (elem->QueryFloatAttribute("aoeModifier", &value) == tinyxml2::XML_SUCCESS)
            item.aoeModifier = value;
        if (elem->QueryFloatAttribute("xpModifier", &value) == tinyxml2::XML_SUCCESS)
            item.xpModifier = value;





        items[item.name] = item;
    }
}


