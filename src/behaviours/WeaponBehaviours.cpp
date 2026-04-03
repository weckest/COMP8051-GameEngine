#include "WeaponBehaviours.h"
#include "World.h"
#include "../manager/TextureManager.h"
#include "manager/AssetManager.h"

float getStat(const Weapon& weapon, const std::string& key, float defaultValue) {
    auto it = weapon.weaponStats.find(key);
    return (it != weapon.weaponStats.end()) ? it->second : defaultValue;
}

std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours {

    // ------------------- Bubble Gun -------------------
    { "bubblegun",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>()) return;

            auto &stats = entity.getComponent<Stats>();
            int count = std::max(1, (int)getStat(weapon, "projectileModifier", 1.0f));

            // Calculate delay between shots based on fire rate upgrades
            float fireRate = WeaponManager::applyItemUpgrade(getStat(weapon, "fireRate", 1.0f), stats.fireRateModifier);
            float delayBetweenShots = 0.15f / fireRate;

            entity.addComponent<TimedSpawner>(
                delayBetweenShots,
                [&weapon, &entity, &world, count, shotsFired = 0, stats]() mutable {

                    if (shotsFired >= count) return;
                    shotsFired++;

                    auto &bullet = world.createDeferredEntity();
                    auto &t = entity.getComponent<Transform>();
                    auto &s = entity.getComponent<Sprite>();

                    SDL_Texture *tex = TextureManager::load("../assets/bubble.png");
                    SDL_FRect src = {0,0,32,32};

                    float sizeMod = WeaponManager::applyItemUpgrade(getStat(weapon, "projectileSizeModifier", 1.0f), stats.projectileSizeModifier);
                    SDL_FRect dst = { t.position.x, t.position.y, src.w * sizeMod, src.h * sizeMod };

                    bullet.addComponent<Sprite>(tex, src, dst);
                    auto &bT = bullet.addComponent<Transform>(
                        Vector2D(t.position.x + s.dst.w/2 - dst.w/2, t.position.y + s.dst.h/2 - dst.h/2),
                        0.0f, 1.0f
                    );

                    Entity* closestEntity = CollisionSystem::getClosestEntity(world, entity, 200);
                    if (!closestEntity) { bullet.destroy(); return; }

                    Vector2D dir = (closestEntity->getComponent<Transform>().position - bT.position).normalize();
                    float speed = 200.0f * WeaponManager::applyItemUpgrade(getStat(weapon, "projectileSpeedModifier", 1.0f), stats.projectileSizeModifier);

                    bullet.addComponent<Velocity>(dir, speed);

                    auto &c = bullet.addComponent<Collider>("bullet");
                    c.rect.w = dst.w; c.rect.h = dst.h;
                    c.layer = CollisionLayer::PROJECTILE;
                    c.mask = CollisionLayer::ENEMY;

                    float damage = 50.0f * WeaponManager::applyItemUpgrade(getStat(weapon, "damageModifier", 1.0f), stats.damageModifier);
                    float aoe = 100.0f * WeaponManager::applyItemUpgrade(getStat(weapon, "aoeModifier", 1.0f), stats.aoeModifier);

                    bullet.addComponent<ProjectileTag>(damage, aoe);
                    bullet.addComponent<Weapon>(weapon);
                    bullet.addComponent<weaponOrigin>(&weapon);
                }
            );
        }
    },

    // ------------------- Shotgun -------------------
    { "shotgun",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>()) return;

            auto &stats = entity.getComponent<Stats>();
            int baseCount = (int)getStat(weapon, "projectileModifier", 1.0f) * 3;
            int count = std::max(1, baseCount);

            float baseAngle = getStat(weapon, "spreadModifier", 1.0f);
            float step = (count > 1) ? (baseAngle / (count - 1)) : 0.0f;

            auto &t = entity.getComponent<Transform>();
            auto &s = entity.getComponent<Sprite>();
            auto &v = entity.getComponent<Velocity>();

            Vector2D forward = v.direction.length() == 0 ? Vector2D(1, 0) : v.direction.normalize();

            SDL_Texture *tex = TextureManager::load("../assets/bullet.png");
            SDL_FRect src = {0, 0, 32, 32};
            float sizeMod = WeaponManager::applyItemUpgrade(getStat(weapon, "projectileSizeModifier", 1.0f), stats.projectileSizeModifier);

            float speedMod = WeaponManager::applyItemUpgrade(getStat(weapon, "projectileSpeedModifier", 1.0f), 0.0f);
            float damageMod = WeaponManager::applyItemUpgrade(getStat(weapon, "damageModifier", 1.0f), stats.damageModifier);
            float aoeMod = WeaponManager::applyItemUpgrade(getStat(weapon, "aoeModifier", 1.0f), stats.aoeModifier);

            for (int i = 0; i < count; i++) {
                auto &bullet = world.createDeferredEntity();

                SDL_FRect dst = { t.position.x, t.position.y, src.w * sizeMod, src.h * sizeMod };
                bullet.addComponent<Sprite>(tex, src, dst);

                float angle = step * ((count - 1) / 2.0f - i);
                auto &bT = bullet.addComponent<Transform>(
                    Vector2D(t.position.x + s.dst.w/2 - dst.w/2, t.position.y + s.dst.h/2 - dst.h/2),
                    angle, 1.0f
                );

                Vector2D dir(forward.x * cos(angle) - forward.y * sin(angle),
                             forward.x * sin(angle) + forward.y * cos(angle));

                bullet.addComponent<Velocity>(dir, 300.0f * speedMod);

                auto &c = bullet.addComponent<Collider>("bullet");
                c.rect.w = dst.w; c.rect.h = dst.h;
                c.layer = CollisionLayer::PROJECTILE;
                c.mask = CollisionLayer::ENEMY;

                bullet.addComponent<ProjectileTag>(50.0f * damageMod, 100.0f * aoeMod);
                bullet.addComponent<Weapon>(weapon);
                bullet.addComponent<weaponOrigin>(&weapon);
            }
        }
    },

    // ------------------- Ring of Fire -------------------
    { "RingofFire",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>()) return;

            auto &stats = entity.getComponent<Stats>();
            int count = std::max(1, (int)getStat(weapon, "projectileModifier", 1.0f));

            auto &playerTransform = entity.getComponent<Transform>();
            auto &playerSprite = entity.getComponent<Sprite>();
            Vector2D centerPos = {playerTransform.position.x + playerSprite.dst.w / 2.0f,
                                  playerTransform.position.y + playerSprite.dst.h / 2.0f};

            float radius = 50.0f * WeaponManager::applyItemUpgrade(getStat(weapon, "rangeModifier", 1.0f), stats.aoeModifier);
            float damage = 25.0f * WeaponManager::applyItemUpgrade(getStat(weapon, "damageModifier", 1.0f), stats.damageModifier);

            for (int i = 0; i < count; ++i) {
                auto &ring = world.createDeferredEntity();

                // Transform
                ring.addComponent<Transform>(Vector2D(centerPos.x - radius, centerPos.y - radius), 0.0f, 1.0f);

                // Collider
                auto& c = ring.addComponent<Collider>("RingoFire");
                c.rect.w = radius * 2.0f; c.rect.h = radius * 2.0f;
                c.layer = CollisionLayer::PROJECTILE;
                c.mask = CollisionLayer::ENEMY;

                // Sprite
                SDL_Texture* tex = TextureManager::load("../assets/animations/ringOfFire.png");
                SDL_FRect src = {0, 0, 64, 64};
                SDL_FRect dst = {centerPos.x - radius, centerPos.y - radius, radius * 2.0f, radius * 2.0f};
                ring.addComponent<Sprite>(tex, src, dst);

                // Animation
                auto& anim = ring.addComponent<Animation>();
                anim = AssetManager::getAnimation("spin_effect");
                anim.currentClip = "spin_effect"; anim.currentFrame = 0; anim.time = 0.0f; anim.speed = 0.05f;

                // RingFireTag
                ring.addComponent<RingFireTag>();
                RingFireTag& RFT = ring.getComponent<RingFireTag>();
                RFT.range = radius;
                RFT.critMultiplier = WeaponManager::applyItemUpgrade(getStat(weapon, "critDamageModifier", 1.5f), stats.damageModifier);
                RFT.critChance = WeaponManager::applyItemUpgrade(getStat(weapon, "critChanceModifier", 0.0f), stats.damageModifier);

                // Lifetime
                ring.addComponent<Lifetime>(getStat(weapon, "lifetime", 0.5f));

                // ProjectileTag
                ring.addComponent<ProjectileTag>(damage, 0.0f);
                ring.addComponent<weaponOrigin>(&weapon);
            }
        }
    }
};

std::function<void(Weapon&, Entity&, World&)> getWeaponBehaviour(const std::string& name) {
    auto it = weaponBehaviours.find(name);
    if (it != weaponBehaviours.end()) return it->second;
    return nullptr;
}