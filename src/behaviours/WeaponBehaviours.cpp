#include "WeaponBehaviours.h"
#include "World.h"
#include "../manager/TextureManager.h"

std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours {
    {
        "bubblegun",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>())
                return;

            int count = std::max(1, (int)weapon.weaponStats.at("projectileModifier"));

            float fireRate = weapon.weaponStats.at("fireRate");
            float fireRateMod = 1.0f + 0.05f * entity.getComponent<Stats>().fireRateModifier;

            float delayBetweenShots = 0.15f / (fireRate * fireRateMod); // tweak for how "disjointed" it feels

            entity.addComponent<TimedSpawner>(
                delayBetweenShots,
                [&, count, shotsFired = 0]() mutable {

                    if (shotsFired >= count) {
                        return; // stop spawning
                    }

                    shotsFired++;

                    auto &bullet = world.createDeferredEntity();
                    auto &t = entity.getComponent<Transform>();
                    auto &s = entity.getComponent<Sprite>();

                    SDL_Texture *tex = TextureManager::load("../assets/bubble.png");

                    SDL_FRect src = {0,0,32,32};
                    SDL_FRect dst = {
                        t.position.x,
                        t.position.y,
                        src.w * weapon.weaponStats.at("projectileSizeModifier"),
                        src.h * weapon.weaponStats.at("projectileSizeModifier"),
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
                        bullet.destroy();
                        return;
                    }

                    auto& eT = closestEntity->getComponent<Transform>();
                    Vector2D dir = (eT.position - bT.position).normalize();

                    bullet.addComponent<Velocity>(dir, 200.0f);

                    auto &c = bullet.addComponent<Collider>("bullet");
                    c.rect.w = dst.w;
                    c.rect.h = dst.h;
                    c.layer = CollisionLayer::PROJECTILE;
                    c.mask = CollisionLayer::ENEMY;

                    bullet.addComponent<ProjectileTag>(
                        50.0f * weapon.weaponStats.at("damageModifier") +
                        (1.0f + 0.05f * entity.getComponent<Stats>().damageModifier),
                        100.0f * weapon.weaponStats.at("aoeModifier")
                    );
                }
            );
        }
    } ,
    { "shotgun",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>())
                return;
           entity.addComponent<TimedSpawner>(
    1.0f / weapon.weaponStats.at("fireRate") + (1.0f + 0.05f * entity.getComponent<Stats>().fireRateModifier),
    [&entity, &world, weapon] {
                int count = std::max(1, (int)(weapon.weaponStats.at("projectileModifier") * 3));

                float baseAngle = weapon.weaponStats.at("aoeModifier"); // total spread in radians

                float step = (count > 1) ? (baseAngle / (count - 1)) : 0.0f;

                auto &t = entity.getComponent<Transform>();
                auto &s = entity.getComponent<Sprite>();
                auto &v = entity.getComponent<Velocity>();

                Vector2D forward = v.direction;
                if (forward.length() == 0) forward = Vector2D(1, 0);
                forward = forward.normalize();

                SDL_Texture *tex = TextureManager::load("../assets/bullet.png");
                SDL_FRect src = {0, 0, 32, 32};

                for (int i = 0; i < count; i++) {
                auto &bullet = world.createDeferredEntity();

                SDL_FRect dst = {
                t.position.x,
                t.position.y,
                src.w * weapon.weaponStats.at("projectileSizeModifier"),
                src.h * weapon.weaponStats.at("projectileSizeModifier"),
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

                bullet.addComponent<ProjectileTag>(
                50.0f * weapon.weaponStats.at("damageModifier") + (1.0f + 0.05f * entity.getComponent<Stats>().damageModifier),
                100.0f * weapon.weaponStats.at("aoeModifier")
                );
                }
    }
);
        }
    },
    {"RingofFire",
        [](Weapon &weapon, Entity &entity, World &world) {
                if (!entity.hasComponent<Collider>()) {
                    return;
                }


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