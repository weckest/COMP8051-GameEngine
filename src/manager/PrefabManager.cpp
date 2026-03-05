//
// Created by Weckest on 2026-03-05.
//

#include "PrefabManager.h"

#include <iostream>

#include "Component.h"
#include "TextureManager.h"
#include "tinyxml2.h"
#include "World.h"

std::unordered_map<std::string, Entity> PrefabManager::prefabs;

void PrefabManager::addPrefab(const std::string& tag, Entity entity) {
    prefabs.emplace(tag, entity);
}

Entity PrefabManager::getPrefab(const std::string &tag) {
    auto it = prefabs.find(tag);
    if (it != prefabs.end()) {
        return it->second;
    }
    std::cerr << "Prefab not found: " << tag << std::endl;
    return Entity{};
}

Entity& PrefabManager::loadPrefab(const std::string &path) {
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
    {
        std::cout << "Failed to load XML\n";
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("Entity");

    if (!root)
    {
        std::cout << "Prefab missing <Entity> root\n";
        return World::createTempEntity();
    }

    auto& entity = World::createTempEntity();

    for (tinyxml2::XMLElement* element = root->FirstChildElement(); element; element = element->NextSiblingElement())
    {
        std::string name = element->Name();

        std::cout << name << std::endl;

        if (name == "Transform")
        {
            float x = element->FloatAttribute("x");
            float y = element->FloatAttribute("y");
            float rotation = element->FloatAttribute("rotation");
            float scale = element->FloatAttribute("scale");

            entity.addComponent<Transform>(
                Vector2D(x,y),
                rotation,
                scale
            );
        }

        else if (name == "Collider")
        {
            std::cout << "doing collider" << std::endl;
            const char* tag = element->Attribute("tag");
            if (!tag) tag = "default";

            auto& collider = entity.addComponent<Collider>(tag);

            element->QueryFloatAttribute("w", &collider.rect.w);
            element->QueryFloatAttribute("h", &collider.rect.h);

            std::cout << "after this" << std::endl;

            if (entity.hasComponent<Transform>())
            {
                auto& transform = entity.getComponent<Transform>();
                collider.rect.x = transform.position.x;
                collider.rect.y = transform.position.y;
            }
        }

        else if (name == "Sprite")
        {
            const char* texturePath = element->Attribute("texture");

            std::cout << texturePath << std::endl;

            int sx = element->IntAttribute("srcX");
            int sy = element->IntAttribute("srcY");
            int sw = element->IntAttribute("srcW");
            int sh = element->IntAttribute("srcH");

            std::cout << "here" << std::endl;



            SDL_Texture* tex = TextureManager::load(texturePath);

            SDL_FRect src { (float)sx, (float)sy, (float)sw, (float)sh };
            SDL_FRect dst {};
            if (entity.hasComponent<Collider>()) {
                auto& collider = entity.getComponent<Collider>();
                dst = {
                    collider.rect.x,
                    collider.rect.y,
                    collider.rect.w,
                    collider.rect.h
                };
            }

            std::cout << "here 2" << std::endl;

            entity.addComponent<Sprite>(tex, src, dst);
        }

        else if (name == "Tag")
        {
            const char* tagName = element->Attribute("name");

            if (std::string(tagName) == "ItemTag")
                entity.addComponent<ItemTag>();
        }
    }

    return entity;
}

void PrefabManager::clean() {
    for (auto& prefab : prefabs) {
        prefab.second.destroy();
    }
    prefabs.clear();
}
