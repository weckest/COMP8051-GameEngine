//
// Created by Weckest on 2026-03-18.
//

#include "DebugRenderSystem.h"
#include "World.h"

void DebugRenderSystem::render(const std::vector<std::unique_ptr<Entity>> &entities, DebugState debugState) {
    Entity* player = world.getPlayer();
    auto& map = world.getMap();
    int scale = map.scale;
    int width = map.width * scale;
    int height = map.height * scale;
    auto& grid = world.getEntityGrid();
    int cols = grid[0].size();
    int rows = grid.size();
    int colWidth = width / grid[0].size();
    int rowHeight = height / grid.size();
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
        TextureManager::drawCircle(playerCenter, 200.0f, 0, 255, 200);
    }


    if (debugState.colliders) {
        for (auto& e : entities) {
            if (e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
                auto& t = e->getComponent<Transform>();
                auto& sprite = e->getComponent<Sprite>();

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

                SDL_Texture* tex = TextureManager::load("../assets/colors.png");
                SDL_FRect src {0,32,32,32};
                SDL_FRect dst {t.position.x - cam.view.x, t.position.y - cam.view.y, sprite.dst.w, sprite.dst.h};

                TextureManager::draw(tex, &src, &dst);

                //collider debugs
                //only show if debug mode is on
                if (e->hasComponent<Collider>()) {
                    auto& collider = e->getComponent<Collider>();

                    Vector2D tl{collider.rect.x - cam.view.x, collider.rect.y - cam.view.y};
                    Vector2D br{collider.rect.x + collider.rect.w - cam.view.x, collider.rect.y + collider.rect.h - cam.view.y};

                    TextureManager::drawRect(tl, br, collider.r, collider.g, collider.b);

                } else if (!e->hasComponent<EffectTag>()) {
                    SDL_Texture* tex = TextureManager::load("../assets/colors.png");
                    SDL_FRect src {0,32,32,32};
                    SDL_FRect dst {t.position.x - cam.view.x, t.position.y - cam.view.y, sprite.dst.w, sprite.dst.h};

                    TextureManager::draw(tex, &src, &dst);
                }
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
