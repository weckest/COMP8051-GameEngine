// //
// // Created by Weckest on 2026-02-04.
// //

#include "World.h"

#include "WeaponBehaviours.h"

World::World() {
    entityGrid = std::vector<std::vector<std::vector<Entity*>>>(
            rows,
            std::vector<std::vector<Entity*>>(cols)
    );



    getEventManager().subscribe(
       [this](const BaseEvent &e) {
           if (e.type != EventType::ToggleDebug) return;

           debugState.debug = !debugState.debug;
           std::cout << "Debug: " << debugState.debug << std::endl;
           for (auto& entity : entities) {
               if (entity->hasComponent<Label>() && entity->getComponent<Label>().type == LabelType::DebugStats) {
                   entity->getComponent<Label>().visible = debugState.debug;
                   auto& children = entity->getComponent<Children>();
                   for (auto& child : children.children) {
                       child->getComponent<Label>().visible = debugState.debug;
                       if (child->hasComponent<Children>()) {
                           for (auto& labelChild : child->getComponent<Children>().children) {
                               labelChild->getComponent<Label>().visible = debugState.debug;
                           }
                       }
                   }
               }
           }

       });

    getEventManager().subscribe(
        [this](const BaseEvent& e) {
            if (e.type != EventType::KeyPress) return;
            const auto& keyPress = static_cast<const KeyPressEvent&>(e);
            if (debugState.debug) {
                switch (keyPress.keyCode) {
                    case SDLK_T:
                        debugState.timer = !debugState.timer;
                        std::cout << "Debug Timer: " << debugState.timer << std::endl;
                        break;
                    case SDLK_L:
                        debugState.level = !debugState.level;
                        std::cout << "Debug Levels: " << debugState.lines << std::endl;
                        break;
                    case SDLK_J:
                        debugState.lines = !debugState.lines;
                        std::cout << "Debug Lines: " << debugState.lines << std::endl;
                        break;
                    case SDLK_C:
                        debugState.colliders = !debugState.colliders;
                        std::cout << "Debug Colliders: " << debugState.colliders << std::endl;
                        break;
                    case SDLK_G:
                        debugState.grid = !debugState.grid;
                        std::cout << "Debug Grid: " << debugState.grid << std::endl;
                        for (auto& entity : entities) {
                            if (entity->hasComponent<Label>() && entity->getComponent<Label>().type == LabelType::Debug) {
                                entity->getComponent<Label>().visible = !entity->getComponent<Label>().visible;
                            }
                        }
                        break;
                    case SDLK_R:
                        debugState.range = !debugState.range;
                        std::cout << "Debug Range: " << debugState.range << std::endl;
                        break;
                    case SDLK_M: {
                        Vector2D center = getPlayer()->getComponent<Transform>().position;
                        center.x += getPlayer()->getComponent<Sprite>().dst.w / 2.0f;
                        center.y += getPlayer()->getComponent<Sprite>().dst.h / 2.0f;
                        getEventManager().emit(SpawnPrefabEvent{"magnet", center});
                        break;
                    }
                    case SDLK_SLASH:
                        debugState.stopSpawn = !debugState.stopSpawn;
                        std::cout << "Debug Stop Spawn: " << debugState.stopSpawn << std::endl;
                        break;
                    case SDLK_PERIOD:
                        debugState.stopEnemies = !debugState.stopEnemies;
                        std::cout << "Debug Stop Enemies: " << debugState.stopEnemies << std::endl;
                        break;
                    case SDLK_N:
                        //kill all enemies by spawning a bubble on them
                        for (auto& entity : entities) {
                            if (entity->hasComponent<EnemyTag>()) {
                                //spawn bubble
                                auto &bullet = this->createDeferredEntity();
                                auto &t = entity->getComponent<Transform>();
                                auto &s = entity->getComponent<Sprite>();

                                SDL_Texture *tex = TextureManager::load("assets/bubble.png");

                                SDL_FRect src = {0,0,32,32};

                                float sizeMod = 1.0f;

                                SDL_FRect dst = {
                                    t.position.x,
                                    t.position.y,
                                    src.w * sizeMod,
                                    src.h * sizeMod,
                                };

                                bullet.addComponent<Sprite>(tex, src, dst);

                                auto& bT = bullet.addComponent<Transform>(
                                    Vector2D(
                                        t.position.x + s.dst.w/2 - dst.w/2,
                                        t.position.y + s.dst.h/2 - dst.h/2
                                    ),
                                    0.0f,
                                    1.0f
                                );

                                Entity* closestEntity = CollisionSystem::getClosestEntity(*this, *entity, 200);
                                if (!closestEntity) {
                                    // world.getEventManager().emit(DeathEvent{&bullet});
                                    bullet.destroy();
                                    return;
                                }

                                // std::cout << "Spawn Bubble " << &bullet << std::endl;

                                auto& eT = closestEntity->getComponent<Transform>();
                                Vector2D dir = (eT.position - bT.position).normalize();

                                // bullet.addComponent<Velocity>(dir, 200.0f);

                                auto &c = bullet.addComponent<Collider>("bullet");
                                c.rect.w = dst.w;
                                c.rect.h = dst.h;
                                    c.layer = CollisionLayer::PROJECTILE;
                                    c.mask = CollisionLayer::ENEMY;

                                float damage =
                                    entity->getComponent<EnemyTag>().health * 5.0f;

                                // float aoe = 100.0f * getStat(weapon, "aoeModifier", 1.0f);

                                bullet.addComponent<ProjectileTag>(damage, 100.0f);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            switch (keyPress.keyCode) {
                case SDLK_P:
                    isPaused = !isPaused;
                    break;
                case SDLK_Q:
                    weaponManager.switchWeapon(*player);
                    break;
                default:
                    break;
            }
    });


}

void World::initDebug() {
    gridSystem.createDebugLabels(*this, &rows, &cols);
    debugRenderSystem.initDebugLabel();
}
