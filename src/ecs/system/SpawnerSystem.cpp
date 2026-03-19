//
// Created by Weckest on 2026-03-05.
//

#include "SpawnerSystem.h"

#include "World.h"

std::unordered_map<std::string, std::function<void(Transform)>> spawners;

SpawnerSystem::SpawnerSystem(World &world) {

        spawners["coin"] = [&world](const Transform &t) {
                auto& coin = world.createDeferredEntity();
                auto& cT = coin.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
                cT.oldPosition = cT.position;
                auto& c = coin.addComponent<Collider>("item");
                c.rect.x = t.position.x;
                c.rect.y = t.position.y;
                c.rect.w = 32;
                c.rect.h = 32;

                //adding texture to the coins
                SDL_Texture* tex = TextureManager::load("../assets/coin.png");
                SDL_FRect tileSrc {0, 0, 32, 32};
                SDL_FRect tileDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
                coin.addComponent<Sprite>(tex, tileSrc, tileDst);
                coin.addComponent<ItemTag>();
                std::cout << coin.hasComponent<ItemTag>() << std::endl;
        };

        world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
                if (e.type != EventType::SpawnEntity) return;
                const auto& spawn = static_cast<const SpawnPrefabEvent&>(e);
                spawners[spawn.name](spawn.transform);
        });
}
