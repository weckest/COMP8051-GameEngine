//
// Created by Weckest on 2026-03-18.
//

#include "DebugRenderSystem.h"
#include "World.h"
#include "manager/AssetManager.h"

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>> &entities, DebugState debugState) {
    Entity* player = world.getPlayer();
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;
    auto& grid = world.getEntityGrid();
    int cols = grid[0].size();
    int rows = grid.size();
    float colWidth = width * 1.0f / grid[0].size();
    float rowHeight = height * 1.0f / grid.size();
    Entity* cameraEntity = nullptr;


    for (auto& e : entities) {
        if (e->hasComponent<Camera>()) {
            cameraEntity = e.get();
            break;
        }
    }

    if (!player) return;
    if (!cameraEntity) return;
    auto& cam = cameraEntity->getComponent<Camera>();

    auto& playerSprite = player->getComponent<Sprite>();
    auto& playerTransform = player->getComponent<Transform>();
    // std::cout << playerTransform.position << "\n" << playerSprite.dst.w << ", " << playerSprite.dst.h << std::endl;
    auto& stats = player->getComponent<Stats>();

    Vector2D playerCenter = playerTransform.position + Vector2D{playerSprite.dst.w / 2, playerSprite.dst.h / 2};
    //move into view
    playerCenter.x -= cam.view.x;
    playerCenter.y -= cam.view.y;

    if (debugState.range) {

        for (auto& e : entities) {
            if (e->hasComponent<RingFireTag>()) {
                TextureManager::drawCircle(playerCenter, e->getComponent<RingFireTag>().range, 0, 255, 200);
            }
        }
    }

    int sumColliders = 0;
    int sumItems = 0;
    int sumEnemies = 0;
    std::vector<Entity*> sumFlagEntities;
    for (auto& e : entities) {
        if (e->hasComponent<Label>()) {
            if (e->getComponent<Label>().type == LabelType::DebugStats) {
                updateDebugLabel(*e);
            }
        }
        if (e->hasComponent<Clickable>()) continue;
        if (e->hasComponent<Collider>()) {
            sumFlagEntities.push_back(e.get());
            sumColliders++;
        }
        if (e->hasComponent<ItemTag>()) {
            sumItems++;
        }
        if (e->hasComponent<EnemyTag>()) {
            sumEnemies++;
        }
    }

    // std::cout << sumColliders << " sum colliders" << std::endl;
    // std::cout << sumItems << " sum items" << std::endl;
    // std::cout << sumEnemies << " sum enemies" << std::endl;
    //
    // int sumGridColliders = 0;
    // int sumGridItems = 0;
    // int sumGridEnemies = 0;
    // std::vector<Entity*> flaggedEntities;
    // std::vector<Entity*> gridFlaggedEntities;
    // for (int i = 0; i < grid.size(); i++) {
    //     for (int j = 0; j < grid[i].size(); j++) {
    //         auto& cell = grid[i][j];
    //         for (auto& e : cell) {
    //             auto it = std::find(flaggedEntities.begin(), flaggedEntities.end(), e);
    //             if (it == flaggedEntities.end()) {
    //                 if (e->hasComponent<Clickable>()) continue;
    //                 flaggedEntities.push_back(e);
    //                 if (e->hasComponent<Collider>()) {
    //                     gridFlaggedEntities.push_back(e);
    //                     sumGridColliders++;
    //                 }
    //                 if (e->hasComponent<ItemTag>()) {
    //                     sumGridItems++;
    //                 }
    //                 if (e->hasComponent<EnemyTag>()) {
    //                     sumGridEnemies++;
    //                 }
    //             }
    //         }
    //     }
    // }

    // std::cout << sumGridColliders << " grid colliders" << std::endl;
    // std::cout << sumGridItems << " grid items" << std::endl;
    // std::cout << sumGridEnemies << " grid enemies" << std::endl;
    //
    // if (sumGridColliders != sumColliders || sumGridEnemies != sumEnemies) {
    //     std::cout << "Sums" << std::endl;
    //     for (auto& e : sumFlagEntities) {
    //         std::cout << e << std::endl;
    //     }
    //
    //     std::cout << "\nGrid" << std::endl;
    //     for (auto& e : gridFlaggedEntities) {
    //         std::cout << e << std::endl;
    //     }
    //
    //     if (sumGridColliders > sumColliders) {
    //         std::cout << "\nGrid Diffs" << std::endl;
    //         for (auto& e: gridFlaggedEntities) {
    //             if (std::find(sumFlagEntities.begin(), sumFlagEntities.end(), e) == sumFlagEntities.end()) {
    //                 std::cout << e << std::endl;
    //             }
    //         }
    //     } else if (sumColliders > sumGridColliders) {
    //         std::cout << "\nSum Diffs" << std::endl;
    //         for (auto& e: sumFlagEntities) {
    //             if (std::find(gridFlaggedEntities.begin(), gridFlaggedEntities.end(), e) == gridFlaggedEntities.end()) {
    //                 std::cout << e << std::endl;
    //             }
    //         }
    //     } else if (sumGridEnemies > sumEnemies) {
    //         std::cout << "\nGrid Enemy Diffs" << std::endl;
    //         for (auto& e: gridFlaggedEntities) {
    //             if (std::find(sumFlagEntities.begin(), sumFlagEntities.end(), e) == sumFlagEntities.end()) {
    //                 std::cout << e << std::endl;
    //             }
    //         }
    //     } else if (sumEnemies > sumGridEnemies) {
    //         std::cout << "\nSum Enemy Diffs" << std::endl;
    //         for (auto& e: sumFlagEntities) {
    //             if (std::find(gridFlaggedEntities.begin(), gridFlaggedEntities.end(), e) == gridFlaggedEntities.end()) {
    //                 std::cout << e << std::endl;
    //             }
    //         }
    //     }
    //
    //
    //     std::cout << "SHITS FUCKED!!!" << std::endl;
    // }


    if (debugState.colliders) {
        for (auto& e : entities) {
            if (e->hasComponent<Transform>()) {
                auto& t = e->getComponent<Transform>();

                if (debugState.grid && e->hasComponent<PlayerTag>()) {
                    GridPosition gridPosition{};
                    GridSystem::getGridIndex(e.get(), width, height, cols, rows, &gridPosition);
                    gridPosition.tl.x *= colWidth;
                    gridPosition.tl.y *= rowHeight;
                    gridPosition.tl.x -= cam.view.x;
                    gridPosition.tl.y -= cam.view.y;
                    gridPosition.br.x *= colWidth;
                    gridPosition.br.x -= cam.view.x;
                    gridPosition.br.y *= rowHeight;
                    gridPosition.br.y -= cam.view.y;
                    TextureManager::drawRect(gridPosition.tl, gridPosition.br, 255, 0, 0);
                }

                if (e->hasComponent<Sprite>() && !e->hasComponent<EffectTag>()) {
                    auto& sprite = e->getComponent<Sprite>();

                    SDL_Texture* tex = TextureManager::load("../assets/colors.png");
                    SDL_FRect src {0,32,32,32};
                    SDL_FRect dst {t.position.x - cam.view.x, t.position.y - cam.view.y, sprite.dst.w, sprite.dst.h};

                    TextureManager::draw(tex, &src, &dst);
                }

                //collider debugs
                //only show if debug mode is on
                if (e->hasComponent<Collider>()) {
                    auto& collider = e->getComponent<Collider>();

                    Vector2D tl{collider.rect.x - cam.view.x, collider.rect.y - cam.view.y};
                    Vector2D br{collider.rect.x + collider.rect.w - cam.view.x, collider.rect.y + collider.rect.h - cam.view.y};

                    TextureManager::drawRect(tl, br, collider.r, collider.g, collider.b);

                }
                // else if (!e->hasComponent<EffectTag>()) {
                //     SDL_Texture* tex = TextureManager::load("../assets/colors.png");
                //     SDL_FRect src {0,32,32,32};
                //     SDL_FRect dst {t.position.x - cam.view.x, t.position.y - cam.view.y, sprite.dst.w, sprite.dst.h};
                //
                //     TextureManager::draw(tex, &src, &dst);
                // }
            }
        }
    }

    if (debugState.lines) {
        for (auto& e : entities) {
            if (e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
                auto& t = e->getComponent<Transform>();
                auto& sprite = e->getComponent<Sprite>();

                if (e->hasComponent<ItemTag>()) {
                    Vector2D aCenter = t.position + Vector2D{(sprite.dst.w / 2), (sprite.dst.h / 2)};
                    float aoe = stats.aoeModifier * 64;
                    aCenter.x -= cam.view.x;
                    aCenter.y -= cam.view.y;

                    Vector2D difference = playerCenter - aCenter;

                    TextureManager::drawLine(aCenter, playerCenter - difference.normalize() * aoe);
                    TextureManager::drawLine(playerCenter - difference.normalize() * aoe, playerCenter, 127, 0, 255);

                }
            }
        }
    }
}

void DebugRenderSystem::initDebugLabel() {
    Label label = {
        "0",
        AssetManager::getFont("arial"),
        {255,255,255,255},
        LabelType::DebugStats,
        "debugStats"
    };
    label.visible = false;

    auto player = world.getPlayer();

    auto& entity = world.createEntity();
    TextureManager::loadLabel(label);
    entity.addComponent<Label>(label);
    entity.addComponent<Transform>(Vector2D(10.0f, 30.0f), 0.0f, 1.0f);
    auto& children = entity.addComponent<Children>();

    auto& levelUp = createChildDebugLabe(entity, LabelType::LevelUp, Vector2D(10.0f, 50.0f));
    auto& health = createChildDebugLabe(entity, LabelType::Health, Vector2D(10.0f, 70.0f));
    auto& weapons = createChildDebugLabe(entity, LabelType::Weapons, Vector2D(10.0f, 90.0f));
    auto& times = createChildDebugLabe(entity, LabelType::Times, Vector2D(10.0f, 110.0f));
    times.addComponent<Children>();
}

void DebugRenderSystem::updateDebugLabel(Entity& entity) {
    auto& parent = entity.getComponent<Label>();
    auto& children = entity.getComponent<Children>();
    auto player = world.getPlayer();
    auto& stats = player->getComponent<Stats>();
    auto& pt = player->getComponent<PlayerTag>();

    parent.text = "Entities: " + std::to_string(world.getEntities().size());
    parent.dirty = true;

    //update the children of the parent label
    for (auto& child: children.children) {
        auto& label = child->getComponent<Label>();
        if (label.type == LabelType::LevelUp) {
            label.text = "LevelUp: " + std::to_string((pt.xp / pt.level / 2));
            label.dirty = true;

        } else if (label.type == LabelType::Health) {
            if (player->hasComponent<Health>()) {
                auto& health = player->getComponent<Health>();
                label.text = "Health: " + std::to_string(health.currentHealth);
                label.dirty = true;
            }
        } else if (label.type == LabelType::Weapons) {
            auto& weaponList = player->getComponent<WeaponList>();
            label.text = "";
            for (Weapon w: weaponList.weapons) {
                label.text += w.name + ", ";
                label.dirty = true;
            }
        } else if (label.type == LabelType::Times) {
            std::vector<std::string> timers = world.getTimer().getTimers();
            label.text = "Timers: " + std::to_string(timers.size());
            label.dirty = true;
            auto& t = child->getComponent<Transform>();
            auto& timerChildren = child->getComponent<Children>();
            if (timerChildren.children.empty()) {
                for (int i = 0; i < timers.size(); i++) {
                    createChildDebugLabe(*child, LabelType::Times, Vector2D(20.0f, t.position.y + 20.0f * (i + 1)))
                    .getComponent<Label>().visible = true;

                }
            }
            for (int i = 0; i < timers.size(); i++) {
                auto& childLabel = timerChildren.children[i]->getComponent<Label>();
                childLabel.text = timers[i] + ": " + std::to_string(world.getTimer().getAvgResult(timers[i])) + "ms";
                childLabel.dirty = true;
            }
        }
    }
}

Entity& DebugRenderSystem::createChildDebugLabe(Entity &parent, LabelType type, Vector2D position) {
    auto& children = parent.getComponent<Children>();

    Label label = {
        "0",
        AssetManager::getFont("arial"),
        {255,255,255,255},
        type,
        "debugLabel"
    };
    label.visible = false;


    auto& newLabel = world.createEntity();
    children.children.emplace_back(&newLabel);
    newLabel.addComponent<Parent>(&newLabel);
    TextureManager::loadLabel(label);
    newLabel.addComponent<Label>(label);
    newLabel.addComponent<Transform>(position, 0.0f, 1.0f);

    return newLabel;
}
