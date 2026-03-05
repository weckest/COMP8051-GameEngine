// //
// // Created by Weckest on 2026-02-04.
// //
//
// #include "World.h"
//
// #include <iostream>
//
// #include "Game.h"
// #include "manager/AssetManager.h"
//
// void callback(const CollisionEvent& collision) {
//     if (collision.entityA == nullptr || collision.entityB == nullptr) return;
//     if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;
//
//     auto& colliderA = collision.entityA->getComponent<Collider>();
//     auto& colliderB = collision.entityB->getComponent<Collider>();
//
//     std::cout << "A collision occurred between entity " << colliderA.tag << " and Entity B " << colliderB.tag << std::endl;
//
// }
//
// World::World() {
//     //subscribe to the collision events
//     eventManager.subscribe([this](const CollisionEvent& collision) {
//
//         Entity* sceneStateEntity = nullptr;
//
//         //find scene state
//         for (auto& e : entities) {
//             if (e->hasComponent<SceneState>()) {
//                 sceneStateEntity = e.get();
//                 break;
//             }
//         }
//
//         if (collision.entityA == nullptr || collision.entityB == nullptr) return;
//
//         if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;
//
//         auto& colliderA = collision.entityA->getComponent<Collider>();
//         auto& colliderB = collision.entityB->getComponent<Collider>();
//
//         Entity* player = nullptr;
//         Entity* item = nullptr;
//         Entity* wall = nullptr;
//         Entity* enemy = nullptr;
//         Entity* bullet = nullptr;
//
//         if (colliderA.tag == "player" && colliderB.tag == "item") {
//             player = collision.entityA;
//             item = collision.entityB;
//         } else if (colliderA.tag == "item" && colliderB.tag == "player") {
//             player = collision.entityB;
//             item = collision.entityA;
//         }
//
//         if (player && item) {
//             item->destroy();
//             //scene state
//             if (sceneStateEntity) {
//                 auto& sceneState = sceneStateEntity->getComponent<SceneState>();
//                 sceneState.coinsCollected++;
//                 if (sceneState.coinsCollected > 1) {
//                     Game::onSceneChangeRequest("level2");
//                 }
//             }
//         }
//
//         player = nullptr;
//         item = nullptr;
//
//         //player vs wall
//         if (colliderA.tag == "player" && colliderB.tag == "wall") {
//             player = collision.entityA;
//             wall = collision.entityB;
//         } else if (colliderA.tag == "wall" && colliderB.tag == "player") {
//             player = collision.entityB;
//             wall = collision.entityA;
//         }
//
//         if (player && wall) {
//             //stop the player
//             auto& t = player->getComponent<Transform>();
//             t.position = t.oldPosition;
//         }
//
//         player = nullptr;
//         wall = nullptr;
//
//         //player vs enemy
//         if (colliderA.tag == "player" && colliderB.tag == "enemy") {
//             player = collision.entityA;
//             enemy = collision.entityB;
//         } else if (colliderA.tag == "enemy" && colliderB.tag == "player") {
//             player = collision.entityB;
//             enemy = collision.entityA;
//         }
//
//         if (player && enemy) {
//             player->destroy();
//             //change scenes defer
//             Game::onSceneChangeRequest("gameover");
//         }
//
//         player = nullptr;
//         enemy = nullptr;
//
//         //bullet vs enemy
//         // std::cout << colliderA.tag << " against " << colliderB.tag << std::endl;
//         if (colliderA.tag == "bullet" && colliderB.tag == "enemy") {
//             bullet = collision.entityA;
//             enemy = collision.entityB;
//         } else if (colliderA.tag == "enemy" && colliderB.tag == "bullet") {
//             bullet = collision.entityB;
//             enemy = collision.entityA;
//         }
//
//         if (bullet && enemy) {
//             //make the coin
//
//             auto& bTag = bullet->getComponent<ProjectileTag>();
//             float distance = 5.0f;
//             std::vector<Entity*> entities = CollisionSystem::getAllWithin(*this, *bullet, bTag.aoe);
//             for (auto& e: entities) {
//                 auto& eTag = enemy->getComponent<EnemyTag>();
//
//                 auto& et = e->getComponent<Transform>();
//
//                 if (e != enemy) {
//                     float distanceToEnemy = (bullet->getComponent<Transform>().position - e->getComponent<Transform>().position).length();
//                     float bulletDamage = bTag.damage * (bTag.aoe - distanceToEnemy) / bTag.aoe;
//                     eTag.health -= bulletDamage;
//                 } else {
//                     eTag.health -= bTag.damage;
//                 }
//
//
//
//                 if (eTag.health <= 0) {
//                     auto& bullet = this->createDeferredEntity();
//                     auto& t = enemy->getComponent<Transform>();
//                     bullet.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
//                     auto& c = bullet.addComponent<Collider>("item");
//                     c.rect.x = t.position.x;
//                     c.rect.y = t.position.y;
//                     c.rect.w = 32;
//                     c.rect.h = 32;
//
//                     //adding texture to the coins
//                     SDL_Texture* tex = TextureManager::load("../assets/coin.png");
//                     SDL_FRect tileSrc {0, 0, 32, 32};
//                     SDL_FRect tileDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
//                     bullet.addComponent<Sprite>(tex, tileSrc, tileDst);
//                     bullet.addComponent<ItemTag>();
//
//                     enemy->destroy();
//                 }
//             }
//
//             //make explosion where the bullet hit the enemy
//             //add a little extra since the sprite is the full size
//             float explosionSize = bTag.aoe * 1.2;
//             auto& explosion = this->createDeferredEntity();
//             auto& bt = bullet->getComponent<Transform>();
//             auto& bs = bullet->getComponent<Sprite>();
//             auto& t = explosion.addComponent<Transform>(bt.position);
//             t.position.x = bt.position.x - bs.dst.w / 2 - (explosionSize / 2 - bs.dst.w / 2);
//             t.position.y = bt.position.y - bs.dst.h / 2 - (explosionSize / 2 - bs.dst.h / 2);
//
//             auto& a = AssetManager::getAnimation("explosion");
//             auto& animation = explosion.addComponent<Animation>(a);
//             animation.speed = 0.075f;
//
//             SDL_Texture* tex = TextureManager::load("../assets/animations/explosion.png");
//             SDL_FRect src {0, 0, 85.33, 85.33};
//             SDL_FRect dst {t.position.x, t.position.y, explosionSize, explosionSize};
//             explosion.addComponent<Sprite>(tex, src, dst);
//
//             explosion.addComponent<EffectTag>();
//
//
//
//             bullet->destroy();
//         }
//
//         bullet = nullptr;
//         enemy = nullptr;
//     });
//
//     // eventManager.subscribe(callback);
// }
//
//
//
