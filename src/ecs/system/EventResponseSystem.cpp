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

            onCollision(collision, "player", "item", world);
            onCollision(collision, "player", "wall", world);
            onCollision(collision, "player", "enemy", world);
            // onCollision(collision, "bullet", "enemy", world);
        }
    );

    world.getEventManager().subscribe(
       [this, &world](const BaseEvent& e) {

           if (e.type != EventType::PlayerAction) return;
           const auto& playerAction = static_cast<const PlayerActionEvent&>(e); //cast base type into collision type

            //TODO onPlayerAction

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

        em.emit(DeathEvent(entityB));
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
    } else if (checkTagsFor(ATag, BTag, "wall") && checkTagsFor(ATag, BTag, "player")) {

        if (e.state != CollisionState::Stay) return;

        auto& t = entityA->getComponent<Transform>();
        t.position = t.oldPosition;

    } else if (checkTagsFor(ATag, BTag, "enemy") && checkTagsFor(ATag, BTag, "player")) {

        if (e.state != CollisionState::Enter) return;

        //this logic is simple and direct
        //ideally we would only operate on data in an update function (hinting at transient entities)
        auto& health = entityA->getComponent<Health>();
        health.currentHealth--;


        Game::gameState.playerHealth = health.currentHealth;

        std::cout << health.currentHealth << std::endl;

        auto& bt = entityB->getComponent<Transform>();
        Vector2D diff = bt.oldPosition - bt.position;

        //could be scaled by knockback modifier
        bt.position += diff * 2;

        if (health.currentHealth <= 0) {
            em.emit(DeathEvent(entityA));
            entityA->destroy();
            Game::onSceneChangeRequest("gameover");
        }

    } else if (checkTagsFor(ATag, BTag, "bullet") && checkTagsFor(ATag, BTag, "enemy")) {

        if (e.state != CollisionState::Enter) return;

         auto& bTag = entityA->getComponent<ProjectileTag>();
         std::vector<Entity*> entities = CollisionSystem::getAllWithin(world, *entityA, bTag.aoe);
         for (auto& e: entities) {

             auto& eTag = e->getComponent<EnemyTag>();

             if (e != entityB) {
                 float distanceToEnemy = (entityA->getComponent<Transform>().position - e->getComponent<Transform>().position).length();
                 float bulletDamage = bTag.damage * (bTag.aoe - distanceToEnemy) / bTag.aoe;
                 eTag.health -= bulletDamage;
             } else {
                 eTag.health -= bTag.damage;
             }



             if (eTag.health <= 0) {
                 world.getEventManager().emit(SpawnPrefabEvent{"coin", entityB->getComponent<Transform>()});

                 //destroy the enemy
                 em.emit(DeathEvent(e));
                 e->destroy();
             }
         }

         //make explosion where the bullet hit the enemy
         //add a little extra since the sprite is the full size
         float explosionSize = bTag.aoe * 1.2;
         auto& explosion = world.createDeferredEntity();
         auto& bt = entityA->getComponent<Transform>();
         auto& bs = entityA->getComponent<Sprite>();
         auto& t = explosion.addComponent<Transform>(bt.position);
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
        em.emit(DeathEvent(entityA));
        entityA->destroy();
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
