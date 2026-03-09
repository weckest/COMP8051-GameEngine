//
// Created by Weckest on 2026-02-25.
//

#include "../scene/Scene.h"

#include "../manager/AssetManager.h"
#include "Game.h"

Scene::Scene(SceneType sceneType, const char *sceneName, const char *mapPath, int windowWidth, int windowHeight)
: name(sceneName), type(sceneType) {

    if (sceneType == SceneType::MainMenu) {

        //camera
        auto& cam = world.createEntity();
        cam.addComponent<Camera>();


        //menu
        auto& menu(world.createEntity());
        auto& menuTransform = menu.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f);

        SDL_Texture* tex = TextureManager::load("../assets/menu.png");
        SDL_FRect src = {0, 0, 800, 600};
        SDL_FRect dst = {menuTransform.position.x, menuTransform.position.y, (float)windowWidth, (float)windowHeight};

        menu.addComponent<Sprite>(tex, src, dst);
        return;

    }

    //load our map
    world.getMap().load(mapPath, TextureManager::load("../assets/spritesheet.png"));
    for (auto& collider: world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto& c = e.addComponent<Collider>("wall");
        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        //just to have a visual of the colliders
        // SDL_Texture* tex = TextureManager::load("../assets/spritesheet.png");
        // SDL_FRect tileSrc {0, 32, 32, 32};
        // SDL_FRect tileDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
        // e.addComponent<Sprite>(tex, tileSrc, tileDst);
    }

    // for (auto& item: world.getMap().items) {
    //     auto& e = world.createEntity();
    //     e.addComponent<Transform>(Vector2D(item.rect.x, item.rect.y), 0.0f, 1.0f);
    //     auto& c = e.addComponent<Collider>("item");
    //     c.rect.x = item.rect.x;
    //     c.rect.y = item.rect.y;
    //     c.rect.w = item.rect.w;
    //     c.rect.h = item.rect.h;
    //
    //     //adding texture to the coins
    //     SDL_Texture* tex = TextureManager::load("../assets/coin.png");
    //     SDL_FRect tileSrc {0, 0, 32, 32};
    //     SDL_FRect tileDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
    //     e.addComponent<Sprite>(tex, tileSrc, tileDst);
    //     e.addComponent<ItemTag>();
    // }

    //add spawners
    for (auto& t: world.getMap().spawners) {
        auto& e(world.createEntity());
        e.addComponent<TimedSpawner>(4.0f, [this, t] {
            //create our projectile (bird)
            auto& e(world.createDeferredEntity());
            e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
            e.addComponent<Velocity>(Vector2D(0.0f,-1.0f), 50.0f);

            auto& anim = AssetManager::getAnimation("enemy");
            e.addComponent<Animation>(anim);

            SDL_Texture* tex = TextureManager::load("../assets/animations/fox_anim.png");
            SDL_FRect src = {0, 0, 32, 32};
            SDL_FRect dst = {t.position.x, t.position.y, 32, 32};
            e.addComponent<Sprite>(tex, src, dst);

            auto& c = e.addComponent<Collider>("enemy");
            c.rect.w = dst.w;
            c.rect.h = dst.h;;

            e.addComponent<EnemyTag>(100.0f);
        });
    }

    // player = new GameObject("../assets/ball.png", 0, 0);

    //add entities
    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.x = 0;
    camView.y = 0;
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, world.getMap().width * 32.0f, world.getMap().height * 32.0f);


    auto& player(world.createEntity());
    auto& pt = player.addComponent<PlayerTag>();
    auto& playerTransform = player.addComponent<Transform>(Vector2D(world.getMap().width * 32 / 2,world.getMap().height * 32 / 2), 0.0f, 0.0f);

    auto& playerVelocity = player.addComponent<Velocity>(Vector2D(0.0f,0.0f), 120.0f * pt.speedModifier);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../assets/animations/spritesheet.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
    SDL_FRect playerDst = {playerTransform.position.x, playerTransform.position.y, 64 * pt.playerSizeModifier, 64 * pt.playerSizeModifier};
    player.addComponent<Sprite>(tex, playerSrc, playerDst);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w;
    playerCollider.rect.h = playerDst.h;

    player.addComponent<Health>(Game::gameState.playerHealth);

    //make the player shoot
    auto& playerGun = player.addComponent<TimedSpawner>(0.25f * 1 + (1 - pt.fireRateModifier),[this, &player, pt] {
        auto& e(world.createDeferredEntity());

        auto& t = player.getComponent<Transform>();
        auto& v = player.getComponent<Velocity>();
        auto& s = player.getComponent<Sprite>();

        SDL_Texture* tex = TextureManager::load("../assets/bubble.png");
        SDL_FRect src = {0, 0, 32, 32};
        SDL_FRect dst = {t.position.x, t.position.y, src.w * pt.projectileSizeModifier, src.h * pt.projectileSizeModifier};
        e.addComponent<Sprite>(tex, src, dst);

        //set the initial position of bullet
        e.addComponent<Transform>(Vector2D(t.position.x + s.dst.w / 2 - dst.w / 2, t.position.y + s.dst.h / 2 - dst.h / 2), 0.0f, 1.0f);
        if (v.direction == Vector2D(0.0f,0.0f)) {
            e.addComponent<Velocity>(Vector2D(1.0f, 0.0f), 200.0f);
        } else {
            e.addComponent<Velocity>(v.direction, 200.0f);
        }

        auto& c = e.addComponent<Collider>("bullet");
        c.rect.w = dst.w;
        c.rect.h = dst.h;
        e.addComponent<ProjectileTag>(50.0f * pt.damageModifier, 100.0f * pt.aoeModifier);
    });



    //add scene state
    auto& state(world.createEntity());
    state.addComponent<SceneState>();

    //subscribe to events

}
