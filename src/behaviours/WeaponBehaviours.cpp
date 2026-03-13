#include "WeaponBehaviours.h"
#include "World.h"
#include "TextureManager.h"

std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours {
    {
        "bubblegun",
        [](Weapon &weapon, Entity &entity, World &world) {
            if (!entity.hasComponent<Collider>())
                return;

            entity.addComponent<TimedSpawner>(
                0.25f * 1 + (1.0f - weapon.fireRate),
                [&entity, &world, weapon] {
                    auto &bullet = world.createDeferredEntity();
                    auto &t = entity.getComponent<Transform>();
                    auto &v = entity.getComponent<Velocity>();
                    auto &s = entity.getComponent<Sprite>();

                    SDL_Texture *tex = TextureManager::load("../assets/bubble.png");

                    SDL_FRect src = {0,0,32,32};
                    SDL_FRect dst = {
                        t.position.x,
                        t.position.y,
                        src.w * weapon.projectileSizeModifier,
                        src.h * weapon.projectileSizeModifier
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
                    if (closestEntity == nullptr) {
                        bullet.destroy();
                        return;
                    }
                    auto& eT = closestEntity->getComponent<Transform>();
                    Vector2D bulletDir = eT.position - bT.position;
                    // Vector2D bulletDir = (v.direction == Vector2D(0,0)) ? Vector2D(1,0) : v.direction;
                    bullet.addComponent<Velocity>(bulletDir, 200.0f);

                    auto &c = bullet.addComponent<Collider>("bullet");
                    c.rect.w = dst.w;
                    c.rect.h = dst.h;

                    bullet.addComponent<ProjectileTag>(
                        50.0f * weapon.damageModifier,
                        100.0f * weapon.aoeModifier
                    );


                }
            );
        }
    } ,
    { "shotgun",
        [](Weapon &weapon, Entity &entity, World &world) {

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