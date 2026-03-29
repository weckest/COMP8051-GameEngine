//
// Created by Weckest on 2026-02-25.
//

#include "../scene/Scene.h"

#include <cstring>

#include "../manager/AssetManager.h"
#include "Game.h"
#include "manager/WeaponManager.h"

Scene::Scene(SceneType sceneType, const char *sceneName, const char *mapPath, int windowWidth, int windowHeight)
: name(sceneName), type(sceneType) {


    if (sceneType == SceneType::MainMenu) {

        initMainMenu(windowWidth, windowHeight);
        return;

    }



    initGameplay(mapPath, windowWidth, windowHeight);

}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
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

    auto& settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
    createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {

    world.getEventManager().subscribe([this, windowWidth, windowHeight](const BaseEvent& e) {
        if (e.type != EventType::ShowLevelUpMenu) return;

        const auto& ev = static_cast<const ShowLevelUpMenuEvent&>(e);

        createLevelUpMenu(windowWidth, windowHeight, ev.weapon, ev.item);
    });





    //load our map
    if (std::strcmp(mapPath, "../assets/map-tlc/TLC-MapUpdated.tmx") == 0) {
        world.getMap().load(mapPath, TextureManager::load("../assets/map-tlc/TLC-Tilesheet.png"));
    } else {
        world.getMap().load(mapPath, TextureManager::load("../assets/spritesheet.png"));
    }


    for (auto& collider: world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto& c = e.addComponent<Collider>("wall");
        // std::cout << "Wall " << &c << std::endl;
        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;
        c.layer = CollisionLayer::WALL;

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

    // add spawners
    for (auto& t: world.getMap().spawners) {
        auto& e(world.createEntity());
        e.addComponent<TimedSpawner>(6.0f, [this, t] {
            //create our projectile (bird)
            auto& e(world.createDeferredEntity());
            // std::cout << "Spawn Enemy " << &e << std::endl;
            e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
            e.addComponent<Velocity>(Vector2D(0.0f,-1.0f), 50.0f);

            auto& anim = AssetManager::getAnimation("enemy");
            e.addComponent<Animation>(anim);

            SDL_Texture* tex = TextureManager::load("../assets/animations/fox_anim.png");
            SDL_FRect src = {0, 0, 32, 32};
            SDL_FRect dst = {t.position.x, t.position.y, 32, 32};
            e.addComponent<Sprite>(tex, src, dst, RenderLayer::World);

            auto& c = e.addComponent<Collider>("enemy");
            // std::cout << "Enemy " << &c << std::endl;
            c.rect.w = dst.w;
            c.rect.h = dst.h;
            c.layer = CollisionLayer::ENEMY;
            c.mask = CollisionLayer::PLAYER | CollisionLayer::WALL | CollisionLayer::PROJECTILE;

            e.addComponent<EnemyTag>(100.0f);
        });
        e.addComponent<Transform>(t);
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


    auto& player = world.createEntity();
    // std::cout << "Player " << &player << std::endl;
    world.setPlayer(&player);
    auto& pt = player.addComponent<PlayerTag>();
    auto& playerStats = player.addComponent<Stats>();

    //LECTURE
    //auto& playerTransform = player.addComponent<Transform>(Vector2D(world.getMap().width * 32 / 2,world.getMap().height * 32 / 2), 0.0f, 0.0f);

    //GAME
    //direct position of middle of intersection
    auto& playerTransform = player.addComponent<Transform>(Vector2D((14 * 32) - 16,(25 * 32) + 16), 0.0f, 1.0f);

    auto& playerVelocity = player.addComponent<Velocity>(Vector2D(0.0f,0.0f), 120.0f * playerStats.speedModifier);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../assets/animations/spritesheet.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
    SDL_FRect playerDst = {playerTransform.position.x, playerTransform.position.y, 64 * playerStats.playerSizeModifier, 64 * playerStats.playerSizeModifier};
    player.addComponent<Sprite>(tex, playerSrc, playerDst, RenderLayer::World);

    // auto& playerCollider = player.addComponent<Collider>("player");
    // playerCollider.rect.w = playerDst.w;
    // playerCollider.rect.h = playerDst.h;

    // GAME: Reduce hitbox size
    //TODO: This will need to change when we configure the new sprite
    auto& playerCollider = player.addComponent<Collider>("player");
    std::cout << "Player " << &playerCollider << std::endl;
    playerCollider.rect.w = playerDst.w / 2;
    playerCollider.rect.h = playerDst.h / 2;
    playerCollider.layer = CollisionLayer::PLAYER;
    playerCollider.mask = CollisionLayer::WALL | CollisionLayer::ENEMY;

    player.addComponent<Health>(Game::gameState.playerHealth);

    //Adds lists for items and weapons for player.
    player.addComponent<ItemList>();
    player.addComponent<WeaponList>();


    // adjust this so it fires through weapon manager.
    //make the player shoot

    player.getComponent<WeaponList>().weapons.push_back(WeaponManager::getRandWeapon());


    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{200, 500}});
    world.getEventManager().emit(SpawnPrefabEvent{"food", Vector2D{300, 500}});



    //add scene state
    auto& state(world.createEntity());
    state.addComponent<SceneState>();

    createPlayerPosLabel();
    world.initDebug();

}

Entity& Scene::createSettingsOverlay(int windowWidth, int windowHeight) {

    auto& overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../assets/ui/settings.jpg");

    SDL_FRect overlaySrc = {0,0,windowWidth*0.85f,windowHeight*0.85f};
    SDL_FRect overlayDst = {
        (float)windowWidth/2 - overlaySrc.w/2,
        (float)windowHeight/2 - overlaySrc.h / 2,
        overlaySrc.w,
        overlaySrc.h};

    overlay.addComponent<Transform>(Vector2D(overlayDst.x, overlayDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDst, RenderLayer::UI,false);

    createSettingsUIComponents(overlay);

    return overlay;


}

Entity& Scene::createCogButton(int windowWidth, int windowHeight, Entity& overlay) {

    auto& cog(world.createEntity());

    auto& cogTransform = cog.addComponent<Transform>(Vector2D((float)windowWidth - 50, (float)windowHeight - 50), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/ui/cog.png");
    SDL_FRect cogSrc = {0, 0, 32, 32};
    SDL_FRect cogDst = {cogTransform.position.x, cogTransform.position.y, cogSrc.w, cogSrc.h};

    cog.addComponent<Sprite>(tex, cogSrc, cogDst, RenderLayer::UI);
    cog.addComponent<Collider>("ui", cogDst);

    auto& clickable = cog.addComponent<Clickable>();

    clickable.onPressed =  [&cogTransform] {
        cogTransform.scale = 0.5f;
    };

    clickable.onReleased =  [this , &cogTransform, &overlay] {
        cogTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };


    clickable.onCancel =  [&cogTransform] {
        cogTransform.scale = 1.0f;
    };

    return cog;

}

void Scene::createSettingsUIComponents(Entity& overlay) {

    if (!overlay.hasComponent<Children>() ) {
        overlay.addComponent<Children>();
    }

    auto& overlayTrans = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTrans.position.x;
    float baseY = overlayTrans.position.y;

    auto& closeButton = world.createEntity();
    auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40,baseY + 10), 0.0f, 0.0f);

    SDL_Texture* tex = TextureManager::load("../assets/ui/close.png");
    SDL_FRect closeSrc = {0, 0, 32, 32};
    SDL_FRect closeDst = {closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};

    closeButton.addComponent<Sprite>(tex, closeSrc, closeDst, RenderLayer::UI, false);

    closeButton.addComponent<Collider>("ui", closeDst);

    auto& clickable = closeButton.addComponent<Clickable>();

    clickable.onPressed = [&closeTransform] {
        closeTransform.scale = 0.5f;
    };

    clickable.onReleased = [this, &overlay, &closeTransform] {
      closeTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };

    clickable.onCancel = [&closeTransform] {
        closeTransform.scale = 1.0f;
    };

    closeButton.addComponent<Parent>(&overlay);
    auto& parentChildren = overlay.getComponent<Children>();

    parentChildren.children.push_back(&closeButton);

}

void Scene::toggleSettingsOverlayVisibility(Entity& overlay) {

    auto& sprite = overlay.getComponent<Sprite>();

    bool newVisibility = !sprite.visible;

    sprite.visible = newVisibility;

    if (overlay.hasComponent<Children>()) {

       auto& children = overlay.getComponent<Children>();

        for (auto& child : children.children) {

            if (child && child->hasComponent<Sprite>()) {

                child->getComponent<Sprite>().visible = newVisibility;

            }

            if (child && child->hasComponent<Collider>()) {

                child->getComponent<Collider>().enabled = newVisibility;

            }

        }
    }
}

//Function to display the menu for player level Up.
Entity& Scene::createLevelUpMenu(int windowWidth, int windowHeight, Weapon w, Item i) {

    //Create overlay entity and load background textures
    auto& overlay(world.createEntity());

    SDL_Texture* bgTex = TextureManager::load("../assets/ui/settings.jpg");

    SDL_FRect bgSrc = {0, 0, windowWidth * 0.7f, windowHeight * 0.7f};
    SDL_FRect bgDst = {
        (float)windowWidth / 2 - bgSrc.w / 2,
        (float)windowHeight / 2 - bgSrc.h / 2,
        bgSrc.w,
        bgSrc.h
    };

    overlay.addComponent<Transform>(Vector2D(bgDst.x, bgDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(bgTex, bgSrc, bgDst, RenderLayer::UI, true);
    overlay.addComponent<Children>();

    //Characteristsics of Buttons and how far apart to put them
    float baseX = bgDst.x;
    float baseY = bgDst.y;

    float spacing = 40.0f;
    float buttonWidth = 150.0f;
    float buttonHeight = 150.0f;

    float totalWidth = buttonWidth * 2 + spacing;

    float startX = baseX + (bgDst.w - totalWidth) / 2;
    float centerY = baseY + bgDst.h / 2 - buttonHeight / 2;



    //
    // ===== WEAPON BUTTON =====
    //

    //Loads weapon texture from path
    auto& weaponButton = world.createEntity();

    auto& weaponTransform = weaponButton.addComponent<Transform>(
        Vector2D(startX, centerY), 0.0f, 1.0f
    );

    SDL_Texture* weaponTex = TextureManager::load(w.path.c_str());


    //Get scale correct and position.

    SDL_FRect weaponSrc = {0, 0, buttonWidth, buttonHeight};
    SDL_FRect weaponDst = {
        weaponTransform.position.x,
        weaponTransform.position.y,
        buttonWidth,
        buttonHeight
    };


    //Add sprite and collider
    weaponButton.addComponent<Sprite>(weaponTex, weaponSrc, weaponDst, RenderLayer::UI, true);
    weaponButton.addComponent<Collider>("ui", weaponDst);


    //Functions for when button is pressed and released.
    auto& weaponClickable = weaponButton.addComponent<Clickable>();

    weaponClickable.onPressed = [&weaponTransform]() {
        weaponTransform.scale = 0.9f;
    };

    weaponClickable.onReleased = [this, &overlay, w, i, &weaponTransform, windowWidth, windowHeight]() {
        weaponTransform.scale = 1.0f;

        world.getEventManager().emit(LevelUpChoiceEvent{true, w, i});

        // hide menu after selection
        toggleSettingsOverlayVisibility(overlay);

        //Update inventory function
        createInventoryUI(windowWidth,windowHeight);

    };

    weaponClickable.onCancel = [&weaponTransform]() {
        weaponTransform.scale = 1.0f;
    };


    //Make parent child relationship and add to overlay
    weaponButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&weaponButton);

    //
    // ===== ITEM BUTTON =====
    //

    //create button,
    auto& itemButton = world.createEntity();

    auto& itemTransform = itemButton.addComponent<Transform>(
        Vector2D(startX + buttonWidth + spacing, centerY), 0.0f, 1.0f
    );

    SDL_Texture* itemTex = TextureManager::load(i.path.c_str());

    SDL_FRect itemSrc = {0, 0, buttonWidth, buttonHeight};
    SDL_FRect itemDst = {
        itemTransform.position.x,
        itemTransform.position.y,
        buttonWidth,
        buttonHeight
    };

    //Add sprites and collider
    itemButton.addComponent<Sprite>(itemTex, itemSrc, itemDst, RenderLayer::UI, true);
    itemButton.addComponent<Collider>("ui", itemDst);


    //Functions for when clicked and released
    auto& itemClickable = itemButton.addComponent<Clickable>();

    itemClickable.onPressed = [&itemTransform]() {
        itemTransform.scale = 0.9f;
    };

    itemClickable.onReleased = [this, &overlay, w, i, &itemTransform, windowWidth, windowHeight]() {
        itemTransform.scale = 1.0f;

        world.getEventManager().emit(LevelUpChoiceEvent{false, w, i});

        toggleSettingsOverlayVisibility(overlay);

        //Call function to redraw items/weapons.
        createInventoryUI(windowWidth,windowHeight);

    };

    itemClickable.onCancel = [&itemTransform]() {
        itemTransform.scale = 1.0f;
    };

    //Add as child of overlay, push to overlay
    itemButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&itemButton);

    return overlay;
}




void Scene::createInventoryUI(int windowWidth, int windowHeight) {

    //
    for (auto& e : world.getEntities()) {
        if (e->hasComponent<InventoryUI>()) {
            e->destroy(); // or mark for removal
        }
    }


    Entity* player = world.getPlayer();
    if (!player) return;

    auto& items = player->getComponent<ItemList>().items;
    auto& weapons = player->getComponent<WeaponList>().weapons;

    float startX = 20.0f;

    float startY =  windowHeight - 80.0f; // bottom-left
    float rowSpacing = 40.0f;
    float spacing = 50.0f;


    // ===== ITEMS =====
    int itemIndex = 0;

    for (auto& item : items) {

        auto& icon = world.createEntity();

        auto& transform = icon.addComponent<Transform>(
            Vector2D(startX + itemIndex * spacing, startY), 0.0f, 1.0f
        );

        SDL_Texture* tex = TextureManager::load(item.path.c_str());

        SDL_FRect src = {0,0,32,32};
        SDL_FRect dst = {transform.position.x, transform.position.y, 32, 32};

        icon.addComponent<Sprite>(tex, src, dst, RenderLayer::UI, true);
        icon.addComponent<InventoryUI>();

        itemIndex++;
    }

    // ===== WEAPONS =====
    int weaponIndex = 0;
    float weaponY = startY - rowSpacing;
    for (auto& weapon : weapons) {

        auto& icon = world.createEntity();

        auto& transform = icon.addComponent<Transform>(
            Vector2D(startX + weaponIndex * spacing, weaponY), 0.0f, 1.0f
        );

        SDL_Texture* tex = TextureManager::load(weapon.path.c_str());

        SDL_FRect src = {0,0,32,32};
        SDL_FRect dst = {transform.position.x, transform.position.y, 32, 32};

        icon.addComponent<Sprite>(tex, src, dst, RenderLayer::UI, true);
        icon.addComponent<InventoryUI>();

        weaponIndex++;
    }
}











Entity& Scene::createPlayerPosLabel() {
    auto& playerPosLabel(world.createEntity());
    Label label = {
        "Test String",
        AssetManager::getFont("arial"),
        {255,255,255,255},
        LabelType::PlayerPosition,
        "playerPos"
    };
    TextureManager::loadLabel(label);
    playerPosLabel.addComponent<Label>(label);
    playerPosLabel.addComponent<Transform>(Vector2D(10, 10), 0.0f, 1.0f);
    return playerPosLabel;
}
