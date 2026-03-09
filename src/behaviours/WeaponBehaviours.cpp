#include "WeaponBehaviours.h"
#include "World.h"
#include "TextureManager.h"

std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours {
    {
        "bubblegun",
        [](Weapon &weapon, Entity &entity, World &world) {
            // Only attach a spawner if the entity has a collider tagged "entity"
            if (!entity.hasComponent<Collider>() || entity.getComponent<Collider>().tag == "player")
                return;

            // Add a single TimedSpawner to the entity
            entity.addComponent<TimedSpawner>(
                0.25f * 1 + (1.0f - weapon.fireRate),  // spawn interval
                [&entity, &world, weapon] {
                    // Create a new bullet entity
                    auto &bullet = world.createDeferredEntity();

                    auto &t = entity.getComponent<Transform>();
                    auto &v = entity.getComponent<Velocity>();
                    auto &s = entity.getComponent<Sprite>();

                    // Load texture
                    SDL_Texture *tex = TextureManager::load("../assets/bubble.png");
                    SDL_FRect src = {0, 0, 32, 32};
                    SDL_FRect dst = {
                        t.position.x,
                        t.position.y,
                        src.w * weapon.projectileSizeModifier,
                        src.h * weapon.projectileSizeModifier
                    };

                    // Add sprite component
                    bullet.addComponent<Sprite>(tex, src, dst);

                    // Set initial position (centered on parent)
                    bullet.addComponent<Transform>(
                        Vector2D(
                            t.position.x + s.dst.w / 2 - dst.w / 2,
                            t.position.y + s.dst.h / 2 - dst.h / 2
                        ),
                        0.0f,
                        1.0f
                    );

                    // Set velocity
                    Vector2D bulletDir = (v.direction == Vector2D(0,0)) ? Vector2D(1,0) : v.direction;
                    bullet.addComponent<Velocity>(bulletDir, 200.0f);

                    // Add collider
                    auto &c = bullet.addComponent<Collider>("bullet");
                    c.rect.w = dst.w;
                    c.rect.h = dst.h;

                    // Add projectile tag
                    bullet.addComponent<ProjectileTag>(
                        50.0f * weapon.damageModifier,
                        100.0f * weapon.aoeModifier
                    );
                }
            );
        }
    }
};