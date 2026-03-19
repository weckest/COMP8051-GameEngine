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

                //adding texture to the coins
                SDL_Texture* tex = TextureManager::load("../assets/coin.png");
                SDL_FRect tileSrc {0, 0, 32, 32};
                SDL_FRect tileDst {cT.position.x, cT.position.y, tileSrc.w, tileSrc.h};
                coin.addComponent<Sprite>(tex, tileSrc, tileDst);
                coin.addComponent<ItemTag>();
        };

        spawners["magnet"] = [&world](const Transform &t) {
                auto& magnet = world.createDeferredEntity();
                auto& mT = magnet.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
                mT.oldPosition = mT.position;

                SDL_Texture* tex = TextureManager::load("../assets/mario.png");
                SDL_FRect tileSrc {0, 0, 32, 44};
                SDL_FRect tileDst {mT.position.x, mT.position.y, tileSrc.w, tileSrc.h};
                magnet.addComponent<Sprite>(tex, tileSrc, tileDst);

                magnet.addComponent<MagnetTag>();
                magnet.addComponent<ItemTag>();
        };

        world.getEventManager().subscribe([this](const BaseEvent& e) {
                if (e.type != EventType::SpawnEntity) return;
                const auto& spawn = static_cast<const SpawnPrefabEvent&>(e);
                spawners[spawn.name](spawn.transform);
        });
}
