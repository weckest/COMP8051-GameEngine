//
// Created by Weckest on 2026-03-04.
//

#include "EventResponseSystem.h"

#include "Game.h"
#include "World.h"
#include "manager/AssetManager.h"

EventResponseSystem::EventResponseSystem(World &world) {
    world.getEventManager().subscribe(
        [this, &world](const BaseEvent& e) {

            if (e.type != EventType::Collision) return;
            const auto& collision = static_cast<const CollisionEvent&>(e); //cast base type into collision type

            //item pickup is handled by PickUpSystem.cpp
            //onCollision(collision, "player", "item", world);

            onCollision(collision, "player", "wall", world);
            onCollision(collision, "enemy", "wall", world);
            onCollision(collision, "player", "enemy", world);
            onCollision(collision, "bullet", "enemy", world);
            onCollision(collision, "RingoFire", "enemy", world);
        }
    );

    world.getEventManager().subscribe(
       [this, &world](const BaseEvent& e) {

           if (e.type != EventType::PlayerAction) return;
           const auto& playerAction = static_cast<const PlayerActionEvent&>(e); //cast base type into collision type

            //TODO onPlayerAction

       }
   );

    world.getEventManager().subscribe(
       [this, &world](const BaseEvent& e) {

           if (e.type != EventType::MouseInteraction) return;
           const auto& mouseEvent = static_cast<const MouseInteractionEvent&>(e);

              onMouseInteraction(mouseEvent);

       }
   );

}

bool EventResponseSystem::checkTagsFor(const char *ATag, const char *BTag, std::string tag) {
    if (ATag == nullptr || BTag == nullptr) return false;

    if (ATag == tag) return true;
    if (BTag == tag) return true;

    return false;
}

void EventResponseSystem::onCollision(
    const CollisionEvent &e,
    const char* ATag,
    const char* BTag,
    World &world) {
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;
    auto& em = world.getEventManager();

    // if (e.entityB->hasComponent<Collider>()) {
    //     std::cout << e.entityB->getComponent<Collider>().tag << std::endl;
    // }

    if (!getCollisionEntities(e, ATag, BTag, entityA, entityB)) return;

    if (checkTagsFor(ATag, BTag, "item") && checkTagsFor(ATag, BTag, "player")) {


        if (e.state != CollisionState::Enter) return;


        ///TEST CODE. NEED TO REMOVE LATER
        auto& stats = entityA->getComponent<PlayerTag>();
        stats.xp += 101;

        ///END OF TEST CODE

        // em.emit(DeathEvent(entityB));
        entityB->destroy();



        // for (auto& entity: world.getEntities()) {
        //     if (!entity->hasComponent<SceneState>()) continue;
        //
        //     auto& sceneState = entity->getComponent<SceneState>();
        //     sceneState.coinsCollected++;
        //     if (sceneState.coinsCollected > 1) {
        //         Game::onSceneChangeRequest("level2");
        //     }
        // }
    } else if ((checkTagsFor(ATag, BTag, "player") || checkTagsFor(ATag, BTag, "enemy")) && checkTagsFor(ATag, BTag, "wall")) {

        if (e.state != CollisionState::Stay) return;

        auto& t = entityA->getComponent<Transform>();
        //t.position = t.oldPosition;

        auto& entityCollider = entityA->getComponent<Collider>();

        // std::cout << "Collision: " << entityA << std::endl;
        auto& wallCollider = entityB->getComponent<Collider>();

        Vector2D temp = t.oldPosition;

        //find overlaps
        float overlapX = std::min(entityCollider.rect.w + entityCollider.rect.x - wallCollider.rect.x,
            wallCollider.rect.w + wallCollider.rect.x - entityCollider.rect.x);
        float overlapY = std::min(entityCollider.rect.h + entityCollider.rect.y - wallCollider.rect.y,
            wallCollider.rect.h + wallCollider.rect.y - entityCollider.rect.y);
        Vector2D oldTemp = t.position;
        //resolve
        if (overlapX < overlapY) {
            // Resolve X only → slide vertically
            if (t.position.x > t.oldPosition.x)
                t.position.x -= overlapX; // moving right
            else
                t.position.x += overlapX; // moving left
        }
        else {
            // Resolve Y only → slide horizontally
            if (t.position.y > t.oldPosition.y)
                t.position.y -= overlapY; // moving down
            else
                t.position.y += overlapY; // moving up
        }

        t.oldPosition = oldTemp;
        // std::cout << "Updating: " << entityA << std::endl;
        world.getEventManager().emit(MoveEvent(entityA));
        t.oldPosition = temp;
        // std::cout << "After: " << entityA << " " << entityA->getComponent<Transform>().oldPosition << std::endl;

    } else if (checkTagsFor(ATag, BTag, "enemy") && checkTagsFor(ATag, BTag, "player")) {

        if (e.state != CollisionState::Enter) return;

        //this logic is simple and direct
        //ideally we would only operate on data in an update function (hinting at transient entities)
        auto& health = entityA->getComponent<Health>();
        health.currentHealth--;


        Game::gameState.playerHealth = health.currentHealth;

        // std::cout << health.currentHealth << std::endl;

        auto& bt = entityB->getComponent<Transform>();
        Vector2D diff = bt.oldPosition - bt.position;

        //could be scaled by knockback modifier
        bt.position += diff * 2;

        if (health.currentHealth <= 0) {
            // em.emit(DeathEvent(entityA));
            entityA->destroy();
            Game::onSceneChangeRequest("mainmenu");
        }

    } else if (checkTagsFor(ATag, BTag, "bullet") && checkTagsFor(ATag, BTag, "enemy")) {

        if (e.state != CollisionState::Enter) return;

         auto& bTag = entityA->getComponent<ProjectileTag>();
         std::vector<Entity*> entities = CollisionSystem::getAllWithin(world, *entityA, bTag.aoe);
         for (auto& entity: entities) {

             auto& eTag = entity->getComponent<EnemyTag>();

             if (entity != entityB) {
                 float distanceToEnemy = (entityA->getComponent<Transform>().position - entity->getComponent<Transform>().position).length();
                 float bulletDamage = bTag.damage * (bTag.aoe - distanceToEnemy) / bTag.aoe;
                 eTag.health -= bulletDamage;
             } else {
                 eTag.health -= bTag.damage;
             }



             if (eTag.health <= 0) {
                 //replace with spawinging in a random object
                 auto& entityT = entity->getComponent<Transform>();
                 auto& entityS = entity->getComponent<Sprite>();
                 Vector2D center = entityT.position;
                 center.x += entityS.dst.w / 2;
                 center.y += entityS.dst.h / 2;
                 world.getEventManager().emit(SpawnPrefabEvent{"coin", center});

                 //destroy the enemy
                 // std::cout << "Dead: " << entity << std::endl;
                 Game::gameState.points += 5;
                 std::cout << Game::gameState.points << std::endl;
                 em.emit(DeathEvent(entity));
                 entity->destroy();
             }
         }

         //make explosion where the bullet hit the enemy
         //add a little extra since the sprite is the full size
         float explosionSize = bTag.aoe * 1.2;
         auto& explosion = world.createDeferredEntity();
         auto& bt = entityA->getComponent<Transform>();
         auto& bs = entityA->getComponent<Sprite>();
         auto& t = explosion.addComponent<Transform>(bt.position, 0.0f, 1.0f);
         t.position.x = bt.position.x - bs.dst.w / 2 - (explosionSize / 2 - bs.dst.w / 2);
         t.position.y = bt.position.y - bs.dst.h / 2 - (explosionSize / 2 - bs.dst.h / 2);

         auto& a = AssetManager::getAnimation("explosion");
         auto& animation = explosion.addComponent<Animation>(a);
         animation.speed = 0.075f;

         SDL_Texture* tex = TextureManager::load("../assets/animations/explosion.png");
         SDL_FRect src {0, 0, 85.33, 85.33};
         SDL_FRect dst {t.position.x, t.position.y, explosionSize, explosionSize};
         explosion.addComponent<Sprite>(tex, src, dst);

         explosion.addComponent<EffectTag>();

        //destroy the bullet
        // em.emit(DeathEvent(entityA));
        entityA->destroy();
    } else if (entityA->hasComponent<RingFireTag>() && checkTagsFor(ATag, BTag, "enemy")) {

        //find player
        Entity* player = nullptr;
        for (auto& e: world.getEntities()) {
            if (e->hasComponent<PlayerTag>()) {
                player = e.get();
                break;
            }
        }
        auto& ringFire = entityA->getComponent<RingFireTag>();
        auto& projectileInfo = entityA->getComponent<ProjectileTag>();
        float range = ringFire.range;

        // find all enemies within range
        if (!player) return;

        std::vector<Entity*> enemies = CollisionSystem::getAllWithin(world, *player, range);
        std::vector<Entity*> toDestroy;

        for (auto& enemy : enemies) {
            if (!enemy->hasComponent<EnemyTag>()) continue;

            auto& eTag = enemy->getComponent<EnemyTag>();
            eTag.health -= projectileInfo.damage;

            if (eTag.health <= 0) {
                Vector2D center{};
                if (enemy->hasComponent<Transform>() && enemy->hasComponent<Sprite>()) {
                    auto& t = enemy->getComponent<Transform>();
                    auto& s = enemy->getComponent<Sprite>();
                    center = t.position;
                    center.x += s.dst.w / 2;
                    center.y += s.dst.h / 2;
                }

                world.getEventManager().emit(SpawnPrefabEvent{"coin", center});
                toDestroy.push_back(enemy);
            }
        }

        // safely destroy all dead enemies after loop
        for (auto& enemy : toDestroy) {
            Game::gameState.points += 5;
            std::cout << Game::gameState.points << std::endl;
            world.getEventManager().emit(DeathEvent(enemy));
            enemy->destroy();
        }
    }
}

bool EventResponseSystem::getCollisionEntities(
    const CollisionEvent &e,
    const char* ATag,
    const char* BTag,
    Entity *&entityA,
    Entity *&entityB
) {

    if (e.entityA == nullptr || e.entityB == nullptr) return false;

    if (!(e.entityA->hasComponent<Collider>() && e.entityB->hasComponent<Collider>())) return false;

    auto& colliderA = e.entityA->getComponent<Collider>();
    auto& colliderB = e.entityB->getComponent<Collider>();

    if (colliderA.tag == ATag && colliderB.tag == BTag) {
        entityA = e.entityA;
        entityB = e.entityB;
    } else if (colliderA.tag == BTag && colliderB.tag == ATag) {
        entityA = e.entityB;
        entityB = e.entityA;
    }

    return entityA && entityB;

}

void EventResponseSystem::onMouseInteraction(const MouseInteractionEvent& e) {
    if (!e.entity->hasComponent<Clickable>()) return;

    auto& clickable = e.entity->getComponent<Clickable>();

    switch (e.state) {
        case MouseInteractionState::Pressed:
            clickable.onPressed();
            break;
        case MouseInteractionState::Released:
            clickable.onReleased();
            break;
        case MouseInteractionState::Cancel:
           clickable.onCancel();
            break;
        default:
            break;
    }
}
