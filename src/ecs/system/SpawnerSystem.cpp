//
// Created by Weckest on 2026-03-05.
//

#include "SpawnerSystem.h"

#include "World.h"

std::unordered_map<std::string, std::function<void(Transform)>> spawners;

SpawnerSystem::SpawnerSystem(World &world) {

        spawners["coin"] = [&world](const Vector2D &v) {
                auto& coin = world.createDeferredEntity();
                auto& cT = coin.addComponent<Transform>(v, 0.0f, 1.0f);
                cT.oldPosition = cT.position;

                //adding texture to the coins
                SDL_Texture* tex = TextureManager::load("../assets/coin.png");
                SDL_FRect tileSrc {0, 0, 32, 32};
                SDL_FRect tileDst {cT.position.x, cT.position.y, tileSrc.w, tileSrc.h};

                cT.position.x -= tileSrc.w / 2;
                cT.position.y -= tileSrc.h / 2;

                coin.addComponent<Sprite>(tex, tileSrc, tileDst);
                coin.addComponent<ItemTag>();
        };

        spawners["magnet"] = [&world](const Vector2D &v) {
                auto& magnet = world.createDeferredEntity();
                auto& mT = magnet.addComponent<Transform>(v, 0.0f, 1.0f);
                mT.oldPosition = mT.position;

                SDL_Texture* tex = TextureManager::load("../assets/mario.png");
                SDL_FRect tileSrc {0, 0, 32, 44};
                SDL_FRect tileDst {mT.position.x, mT.position.y, tileSrc.w, tileSrc.h};

                mT.position.x -= tileSrc.w / 2;
                mT.position.y -= tileSrc.h / 2;

                magnet.addComponent<Sprite>(tex, tileSrc, tileDst);

                magnet.addComponent<MagnetTag>();
                magnet.addComponent<ItemTag>();
        };

        spawners["food"] = [&world](const Vector2D &v) {
                auto& magnet = world.createDeferredEntity();
                auto& mT = magnet.addComponent<Transform>(v, 0.0f, 1.0f);
                mT.oldPosition = mT.position;

                SDL_Texture* tex = TextureManager::load("../assets/colors.png");
                SDL_FRect tileSrc {32, 0, 32, 32};
                SDL_FRect tileDst {mT.position.x, mT.position.y, tileSrc.w, tileSrc.h};

                mT.position.x -= tileSrc.w / 2;
                mT.position.y -= tileSrc.h / 2;

                magnet.addComponent<Sprite>(tex, tileSrc, tileDst);

                magnet.addComponent<FoodTag>(30.0f);
                magnet.addComponent<ItemTag>();
        };

        world.getEventManager().subscribe([this](const BaseEvent& e) {
                if (e.type != EventType::SpawnEntity) return;
                const auto& spawn = static_cast<const SpawnPrefabEvent&>(e);
                if (spawn.rand) {
                        SpawnRandomItem(spawn.position);
                } else {
                        spawners[spawn.name](spawn.position);
                }
        });
}

void SpawnerSystem::SpawnRandomItem(const Vector2D &position) {
        auto it = spawners.begin();
        std::advance(it, rand() % spawners.size());
        it->second(position);
}
