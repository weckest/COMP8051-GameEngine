#include "WeaponBehaviours.h"
#include "World.h"
#include "../manager/TextureManager.h"

float getStat(const Weapon& weapon, const std::string& key, float defaultValue) {
    auto it = weapon.weaponStats.find(key);
    return (it != weapon.weaponStats.end()) ? it->second : defaultValue;
}

std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours {
    {
        "bubblegun",
        [](Weapon &weapon, Entity &entity, World &world) {
             if (!entity.hasComponent<Collider>())
            return;

        int count = std::max(1, (int)getStat(weapon, "projectileModifier", 1.0f));

        float fireRate = getStat(weapon, "fireRate", 1.0f);
        float fireRateMod = 1.0f + 0.05f * entity.getComponent<Stats>().fireRateModifier;

        float delayBetweenShots = 0.15f / (fireRate * fireRateMod);

        entity.addComponent<TimedSpawner>(
            delayBetweenShots,
            [&, entity,weapon,count, shotsFired = 0]() mutable {

                if (shotsFired >= count) return;
                shotsFired++;

                auto &bullet = world.createDeferredEntity();
                auto &t = entity.getComponent<Transform>();
                auto &s = entity.getComponent<Sprite>();

                SDL_Texture *tex = TextureManager::load("../assets/bubble.png");

                SDL_FRect src = {0,0,32,32};

                float sizeMod = getStat(weapon, "projectileSizeModifier", 1.0f);

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

                Entity* closestEntity = CollisionSystem::getClosestEntity(world, entity, 200);
                if (!closestEntity) {
                    // world.getEventManager().emit(DeathEvent{&bullet});
                    bullet.destroy();
                    return;
                }

                // std::cout << "Spawn Bubble " << &bullet << std::endl;

                auto& eT = closestEntity->getComponent<Transform>();
                Vector2D dir = (eT.position - bT.position).normalize();

                bullet.addComponent<Velocity>(dir, 200.0f);

                auto &c = bullet.addComponent<Collider>("bullet");
                c.rect.w = dst.w;
                c.rect.h = dst.h;
                    c.layer = CollisionLayer::PROJECTILE;
                    c.mask = CollisionLayer::ENEMY;

                float damage =
                    50.0f * (getStat(weapon, "damageModifier", 1.0f) +
                    (1.0f + 0.05f * entity.getComponent<Stats>().damageModifier));

                float aoe = 100.0f * getStat(weapon, "aoeModifier", 1.0f);

                bullet.addComponent<ProjectileTag>(damage, aoe);
            }
        );
        }
    } ,
    { "shotgun",
        [](Weapon &weapon, Entity &entity, World &world) {
           if (!entity.hasComponent<Collider>())
            return;

        float fireRate = getStat(weapon, "fireRate", 1.0f);

        entity.addComponent<TimedSpawner>(
            1.0f / fireRate + (1.0f + 0.05f * entity.getComponent<Stats>().fireRateModifier),
            [&entity, &world, weapon] {

                int count = std::max(1, (int)(getStat(weapon, "projectileModifier", 1.0f) * 3));

                float baseAngle = getStat(weapon, "spreadModifier", 1.0f);
                float step = (count > 1) ? (baseAngle / (count - 1)) : 0.0f;

                auto &t = entity.getComponent<Transform>();
                auto &s = entity.getComponent<Sprite>();
                auto &v = entity.getComponent<Velocity>();

                Vector2D forward = v.direction;
                if (forward.length() == 0) forward = Vector2D(1, 0);
                forward = forward.normalize();

                SDL_Texture *tex = TextureManager::load("../assets/bullet.png");
                SDL_FRect src = {0, 0, 32, 32};

                float sizeMod = getStat(weapon, "projectileSizeModifier", 1.0f);

                for (int i = 0; i < count; i++) {

                    auto &bullet = world.createDeferredEntity();
                    // std::cout << "Spawn Shot " << &bullet << std::endl;

                    SDL_FRect dst = {
                        t.position.x,
                        t.position.y,
                        src.w * sizeMod,
                        src.h * sizeMod,
                    };

                    bullet.addComponent<Sprite>(tex, src, dst);

                    auto &bT = bullet.addComponent<Transform>(
                        Vector2D(
                            t.position.x + s.dst.w/2 - dst.w/2,
                            t.position.y + s.dst.h/2 - dst.h/2
                        ),
                        0.0f,
                        1.0f
                    );

                    float angle = step * ((count - 1) / 2.0f - i);

                    Vector2D dir(
                        forward.x * cos(angle) - forward.y * sin(angle),
                        forward.x * sin(angle) + forward.y * cos(angle)
                    );

                    bullet.addComponent<Velocity>(dir, 300.0f);

                    auto &c = bullet.addComponent<Collider>("bullet");
                    c.rect.w = dst.w;
                    c.rect.h = dst.h;
                c.layer = CollisionLayer::PROJECTILE;
                c.mask = CollisionLayer::ENEMY;

                    float damage =
                        50.0f * (getStat(weapon, "damageModifier", 1.0f) +
                        (1.0f + 0.05f * entity.getComponent<Stats>().damageModifier));

                    float aoe = 100.0f * getStat(weapon, "aoeModifier", 1.0f);

                    bullet.addComponent<ProjectileTag>(damage, aoe);
                }
            }
        );
        }
    },
    {"RingofFire",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>())
                return;

            int count = std::max(1, (int)getStat(weapon, "projectileModifier", 1.0f));

            float fireRate = getStat(weapon, "fireRate", 1.0f);
            float fireRateMod = 1.0f + 0.05f * entity.getComponent<Stats>().fireRateModifier;

            float delayBetweenShots = 0.15f / (fireRate * fireRateMod);

            entity.addComponent<TimedSpawner>(
                delayBetweenShots,
                [&, entity,weapon,count, shotsFired = 0]() mutable {

                    if (shotsFired >= count) return;
                    shotsFired++;

                    auto &ring = world.createDeferredEntity();

                    auto& playerTransform = entity.getComponent<Transform>();
                    auto& playerSprite = entity.getComponent<Sprite>();

                    ring.addComponent<Transform>(playerTransform.position, 0.0f, 1.0f);

                    auto& c = ring.addComponent<Collider>("RingoFire");
                    // std::cout << "RoF " << &c << std::endl;

                    float radius = 50.0f * getStat(weapon, "rangeModifier", 1.0f);

                    c.rect.w = radius * 2;
                    c.rect.h = radius * 2;
                    c.layer = CollisionLayer::PROJECTILE;
                    c.mask = CollisionLayer::ENEMY;

                    Vector2D centerPos = {
                        playerTransform.position.x + playerSprite.dst.w / 2.0f,
                        playerTransform.position.y + playerSprite.dst.h / 2.0f
                    };

                    ring.getComponent<Transform>().position = {
                        centerPos.x - c.rect.w / 2.0f,
                        centerPos.y - c.rect.h / 2.0f
                    };

                    ring.addComponent<RingFireTag>(radius );

                    // lifetime instead of nested spawner
                    ring.addComponent<Lifetime>(getStat(weapon, "lifetime", 0.1f));

                    ring.addComponent<ProjectileTag>(
                        25.0f * (getStat(weapon, "damageModifier", 1.0f) + (1.0f + 0.05f * entity.getComponent<Stats>().damageModifier)),
                        0.0f
                        );

                }
            );
            // periodically check for all entities within the collider using
            // the collision system

            // do damage

            //destroy itself
    }
    }

};

std::function<void(Weapon&, Entity&, World&)> getWeaponBehaviour(const std::string& name)
{
    auto it = weaponBehaviours.find(name);

    if (it != weaponBehaviours.end())
        return it->second;

    return nullptr;
}
