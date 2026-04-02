//
// Created by Weckest on 2026-02-25.
//

#include "../scene/Scene.h"

#include <cstring>

#include "../manager/AssetManager.h"
#include "Game.h"
#include "manager/WeaponManager.h"

Scene::Scene(SceneType sceneType, const char* sceneName, const char* mapPath,
             int windowWidth, int windowHeight, SDL_Window* window)
    : name(sceneName), type(sceneType) {

    if (sceneType == SceneType::MainMenu) {
        initMainMenu(windowWidth, windowHeight);
        return;
    }

    if (sceneType == SceneType::GameOver) {
        initGameOver(windowWidth, windowHeight);
        return;
    }

    initGameplay(window, mapPath, windowWidth, windowHeight); // now works
}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
    //camera
    auto& cam = world.createEntity();
    cam.addComponent<Camera>();


    //menu
    auto& menu(world.createEntity());
    auto& menuTransform = menu.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/menu/TFC-BG.png");
    SDL_FRect src = {0, 0, 2304, 1296};
    SDL_FRect dst = {menuTransform.position.x, menuTransform.position.y, (float)windowWidth, (float)windowHeight};

    menu.addComponent<Sprite>(tex, src, dst);

    //TODO: Settings and Credits

    //TITLE
    auto& title = world.createEntity();
    auto& titleTransform = title.addComponent<Transform>(Vector2D((windowWidth / 4.0f), 30), 0.0f, 1.0f);

    SDL_Texture* titleTex = TextureManager::load("../assets/menu/TFC-Title.png");
    SDL_FRect titleSrc = {0, 0, 1280, 720};
    SDL_FRect titleDst = {titleTransform.position.x, titleTransform.position.y, (float)windowWidth / 2, (float)windowHeight / 3};
    title.addComponent<Sprite>(titleTex, titleSrc, titleDst);

    title.addComponent<ItemTag>();

    //PLAY
    auto& playButton = makeGenericButton("green", windowHeight / 2.0f, windowWidth);
    auto& playSprite = playButton.getComponent<Sprite>();
    SDL_Texture *playNormal = playSprite.texture;
    auto& pClick = playButton.getComponent<Clickable>();

    pClick.onReleased = [&playSprite, playNormal] {
        playSprite.texture = playNormal;
        Game::onSceneChangeRequest("gameplay");
    };

    //SETTINGS
    auto& setButton = makeGenericButton("grey", windowHeight / 2.0f + 70, windowWidth);
    auto& setSprite = setButton.getComponent<Sprite>();
    SDL_Texture *setNormal = setSprite.texture;
    auto& setClick = setButton.getComponent<Clickable>();

    //CREDITS
    auto& credButton = makeGenericButton("grey", windowHeight / 2.0f + 140, windowWidth);
    auto& credSprite = credButton.getComponent<Sprite>();
    SDL_Texture *credNormal = credSprite.texture;
    auto& credClick = credButton.getComponent<Clickable>();

    //QUIT
    auto& quitButton = makeGenericButton("red", windowHeight / 2.0f + 210, windowWidth);
    auto& quitSprite = quitButton.getComponent<Sprite>();
    SDL_Texture *quitNormal = quitSprite.texture;
    auto& quitClick = quitButton.getComponent<Clickable>();

    quitClick.onReleased = [&quitSprite, quitNormal] {
        quitSprite.texture = quitNormal;
        Game::onSceneChangeRequest("quit");
    };

    //OVERLAYS
    auto& setBox = createSettingsBox(windowWidth, windowHeight);

    setClick.onReleased = [this, &setSprite, setNormal, &setBox] {
        toggleSettingsOverlayVisibility(setBox);
        setSprite.texture = setNormal;
    };

    // credClick.onReleased = [this, &credSprite, credNormal, &credBox] {
    //     //Game::onSceneChangeRequest("gameplay");
    //     //toggleCreditsOverlayVisibility(overlay);
    //     credSprite.texture = credNormal;
    // };

    //COLLIDER HANDLING
    menuButtonColliders.push_back(&playButton.getComponent<Collider>());
    menuButtonColliders.push_back(&setButton.getComponent<Collider>());
    menuButtonColliders.push_back(&credButton.getComponent<Collider>());
    menuButtonColliders.push_back(&quitButton.getComponent<Collider>());

    auto& settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
    createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initGameOver(int windowWidth, int windowHeight) {
    //camera
    auto& cam = world.createEntity();
    cam.addComponent<Camera>();

    //menu
    auto& menu(world.createEntity());
    auto& menuTransform = menu.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/menu/TFC-GameOverBG.png");
    SDL_FRect src = {0, 0, 2304, 1296};
    SDL_FRect dst = {menuTransform.position.x, menuTransform.position.y, (float)windowWidth, (float)windowHeight};

    menu.addComponent<Sprite>(tex, src, dst);

    //Header
    auto& titleText = world.createEntity();
    titleText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 120, windowHeight / 6.0f), 0.0f, 1.0f);
    Label tLabel = {
        "Game Over",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "gameovertitle"
    };
    auto& titleLabel = titleText.addComponent<Label>(tLabel);
    titleLabel.dirty = true;
    TextureManager::loadLabel(tLabel);

    //Stat 1
    auto& stat1Text = world.createEntity();
    stat1Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 146, windowHeight / 3.0f), 0.0f, 1.0f);

    int totalSeconds = static_cast<int>(Game::gameState.time / 1000.0f);
    int remainder = totalSeconds % 60;


    Label s1Label = {
        "Time Survived: " + std::to_string(totalSeconds / 60) + ":" + (remainder < 10 ? "0" : "") + std::to_string(remainder),
        AssetManager::getFont("monogram-medium"),
        {255,255,255,255},
        LabelType::UI,
        "gameoverstat1"
    };
    auto& stat1Label = stat1Text.addComponent<Label>(s1Label);
    stat1Label.dirty = true;
    TextureManager::loadLabel(stat1Label);

    //Stat 2
    auto& stat2Text = world.createEntity();
    stat2Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 131, windowHeight / 3.0f + 50), 0.0f, 1.0f);

    float totalDamage = 0;
    for (const auto& e : Game::gameState.WeaponDamage) {
        totalDamage += e.second;
    }

    Label s2Label = {
        "Total Damage: " + std::format("{:.2f}", totalDamage),
        AssetManager::getFont("monogram-medium"),
        {255,255,255,255},
        LabelType::UI,
        "gameoverstat2"
    };
    auto& stat2Label = stat2Text.addComponent<Label>(s2Label);
    stat2Label.dirty = true;
    TextureManager::loadLabel(stat2Label);

    //Stat 3
    auto& stat3Text = world.createEntity();
    stat3Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 116, windowHeight / 3.0f + 150), 0.0f, 1.0f);
    Label s3Label = {
        "Total Score: " + std::to_string(Game::gameState.points),
        AssetManager::getFont("monogram-medium"),
        {255,255,255,255},
        LabelType::UI,
        "gameoverstat3"
    };
    auto& stat3Label = stat3Text.addComponent<Label>(s3Label);
    stat3Label.dirty = true;
    TextureManager::loadLabel(stat3Label);

    //Continue
    auto& continueText = world.createEntity();
    continueText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 305, windowHeight / 5.0f * 3.5f), 0.0f, 1.0f);
    Label contLabel = {
        "Press Space To Continue",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "gameovercontinue"
    };
    auto& continueLabel = continueText.addComponent<Label>(contLabel);
    continueLabel.dirty = true;
    TextureManager::loadLabel(continueLabel);
}

void Scene::toggleColliders(bool isVisible) {
    if (menuButtonColliders.empty()) return;

    for (auto& e : menuButtonColliders) {
        e->enabled = !isVisible;
    }
}

Entity& Scene::makeGenericButton(const std::string& color, int buttonHeight, int windowWidth) {
    auto& button = world.createEntity();
    auto& bt = button.addComponent<Transform>(Vector2D((windowWidth / 2.0f), buttonHeight), 0.0f, 1.0f);

    SDL_Texture* bTexNormal = TextureManager::load(("../assets/ui/buttons/" + color + "/normal.png").c_str());
    SDL_FRect bSrc = {0, 0, 48, 16};
    SDL_FRect bDst = {bt.position.x, bt.position.y, (float)bSrc.w * 2.5f, (float)bSrc.h * 2.5f};
    bt.position.x -= bDst.w / 2; //center it
    bDst.x -= bDst.w / 2; //center it
    auto& bs = button.addComponent<Sprite>(bTexNormal, bSrc, bDst, RenderLayer::UI);
    button.addComponent<Collider>("ui", bDst);

    auto& bc = button.addComponent<Clickable>();
    SDL_Texture* bTexPressed = TextureManager::load(("../assets/ui/buttons/" + color + "/clicked.png").c_str());

    bc.onPressed =  [&bs, bTexPressed] {
        bs.texture = bTexPressed;
    };

    bc.onCancel =  [&bs, bTexNormal] {
        bs.texture = bTexNormal;
    };

    return button;
}

Entity& Scene::createSettingsBox(int windowWidth, int windowHeight) {
    auto& overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../assets/ui/TFC-MenuBox.png");

    SDL_FRect overlaySrc = {0,0,windowWidth*0.60f,windowHeight*0.80f};
    SDL_FRect overlayDst = {
        (float)windowWidth/2 - overlaySrc.w/2,
        (float)windowHeight/2 - overlaySrc.h / 2,
        overlaySrc.w,
        overlaySrc.h};

    overlay.addComponent<Transform>(Vector2D(overlayDst.x, overlayDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDst, RenderLayer::UI,false);

    createSettingsComponents(overlay);

    return overlay;
}

void Scene::createSettingsComponents(Entity &overlay) {
    if (!overlay.hasComponent<Children>() ) {
        overlay.addComponent<Children>();
    }

    auto& overlayTrans = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTrans.position.x;
    float baseY = overlayTrans.position.y;

    auto& closeButton = world.createEntity();
    auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40,baseY + 10), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/ui/close.png");
    SDL_FRect closeSrc = {0, 0, 32, 32};
    SDL_FRect closeDst = {closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};

    closeButton.addComponent<Sprite>(tex, closeSrc, closeDst, RenderLayer::UI, false);

    closeButton.addComponent<Collider>("ui", closeDst, false);

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

Entity& Scene::createCreditsBox(int windowWidth, int windowHeight) {
    auto& overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../assets/ui/TFC-MenuBox.jpg");

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

void Scene::createCreditsComponents(Entity &overlay) {
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

void Scene::initGameplay(SDL_Window* window, const char* mapPath, int windowWidth, int windowHeight) {

    Game::gameState.points = 0;
    Game::gameState.time = 0.0f;


    world.getEventManager().subscribe([this, window](const BaseEvent& e) {
        if (e.type != EventType::ShowLevelUpMenu) return;

        const auto& ev = static_cast<const ShowLevelUpMenuEvent&>(e);

        // Dynamically get the current window size

        createLevelUpMenu(800, 600, ev.bundle, ev.item);
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

            int zType = rand() % 4;
            SDL_Texture* tex = TextureManager::load(("../assets/animations/z" + std::to_string(zType + 1) + "_anim.png").c_str());
            SDL_FRect src = {0, 0, 128, 128};
            SDL_FRect dst = {t.position.x, t.position.y, 64, 64};
            e.addComponent<Sprite>(tex, src, dst, RenderLayer::World);

            auto& c = e.addComponent<Collider>("enemy");
            // std::cout << "Enemy " << &c << std::endl;
            c.rect.w = dst.w / 2;
            c.rect.h = dst.h / 2;
            c.layer = CollisionLayer::ENEMY;
            c.mask = CollisionLayer::PLAYER | CollisionLayer::WALL | CollisionLayer::PROJECTILE;

            auto& h = e.addComponent<EnemyTag>(100.0f * (1.0f + (Game::gameState.time / 500.0f)));
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

    SDL_Texture* tex = TextureManager::load("../assets/animations/player_anim.png");
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

    player.getComponent<WeaponList>().weapons.push_back(WeaponManager::getRandWeapon().weapon);


    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{830, 120}});
    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{1180, 990}});
    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{1210, 280}});
    world.getEventManager().emit(SpawnPrefabEvent{"food", Vector2D{190, 120}});



    //add scene state
    auto& state(world.createEntity());
    state.addComponent<SceneState>();

    // createPlayerPosLabel();
    createPointsLabel();
    world.initDebug();
    createInventoryUI(windowWidth, windowHeight);
    createLevelUpUI(windowWidth, windowHeight);
    createHealthBar(windowWidth, windowHeight);

}

//LECTURE
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

//LECTURE
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

    closeButton.addComponent<Collider>("ui", closeDst, false);

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

    //auto& children = overlay.getComponent<Children>().children;

    bool newVisibility = !sprite.visible;

    toggleColliders(newVisibility);

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

            if (child && child->hasComponent<Label>()) {
                child->getComponent<Label>().visible = newVisibility;
            }


        }
    }
}

//Function to display the menu for player level Up.
Entity& Scene::createLevelUpMenu(int windowWidth, int windowHeight, dataBundle w, Item i) {

    // DESIGN RESOLUTION (matches SDL logical presentation)
    constexpr int DESIGN_WIDTH = 800;
    constexpr int DESIGN_HEIGHT = 600;

    // ===== OVERLAY =====
    auto& overlay = world.createEntity();

    SDL_Texture* bgTex = TextureManager::load("../assets/ui/settings.jpg");

    float bgWidth = DESIGN_WIDTH * 0.7f;
    float bgHeight = DESIGN_HEIGHT * 0.7f;

    SDL_FRect bgDst = {
        (DESIGN_WIDTH - bgWidth) / 2.0f,
        (DESIGN_HEIGHT - bgHeight) / 2.0f,
        bgWidth,
        bgHeight
    };

    SDL_FRect bgSrc = {0, 0, bgWidth, bgHeight};

    overlay.addComponent<Transform>(Vector2D(bgDst.x, bgDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(bgTex, bgSrc, bgDst, RenderLayer::UI, true);
    overlay.addComponent<Children>();

    // ===== BUTTON LAYOUT =====
    constexpr float BUTTON_SIZE = 120.0f;
    constexpr float SPACING = 60.0f;

    float totalWidth = BUTTON_SIZE * 2 + SPACING;

    float startX = bgDst.x + (bgDst.w - totalWidth) / 2.0f;
    float centerY = bgDst.y + bgDst.h / 2.0f - BUTTON_SIZE / 2.0f;

    // =========================================================
    // WEAPON BUTTON
    // =========================================================
    auto& weaponButton = world.createEntity();

    auto& weaponTransform = weaponButton.addComponent<Transform>(
        Vector2D(startX, centerY), 0.0f, 1.0f
    );

    SDL_Texture* weaponTex = TextureManager::load(w.weapon.path.c_str());

    SDL_FRect weaponDst = {
        weaponTransform.position.x,
        weaponTransform.position.y,
        BUTTON_SIZE,
        BUTTON_SIZE
    };

    SDL_FRect weaponSrc = {0, 0, 32, 32};

    weaponButton.addComponent<Sprite>(weaponTex, weaponSrc, weaponDst, RenderLayer::UI, true);
    weaponButton.addComponent<Collider>("ui", weaponDst);

    //Weapon Label
    auto& weaponLabelEntity = world.createEntity();

    Label weaponLabel = {
        w.weapon.name,
        AssetManager::getFont("bungee"),
        {0,0,0,255},
        LabelType::UI,
        "weaponLabel"
    };

    weaponLabel.textureCacheKey = "weapon_" + w.weapon.name;
    weaponLabel.dirty = true;

    TextureManager::loadLabel(weaponLabel);

    float weaponLabelX = weaponDst.x + BUTTON_SIZE / 2 - weaponLabel.dst.w / 2;
    float weaponLabelY = weaponDst.y - weaponLabel.dst.h - 10;

    weaponLabelEntity.addComponent<Label>(weaponLabel);
    weaponLabelEntity.addComponent<Transform>(Vector2D(weaponLabelX, weaponLabelY), 0.0f, 1.0f);

    weaponLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&weaponLabelEntity);

    //Stat Label
    auto& weaponStatEntity = world.createEntity();

    Label weaponStat = {
        "+" + w.name,
        AssetManager::getFont("bungeeSmall"),
        {0,100,0,255},
        LabelType::UI,
        "weaponStat"
    };

    weaponStat.dirty = true;

    TextureManager::loadLabel(weaponStat);

    float weaponStatX = weaponDst.x + BUTTON_SIZE / 2 - weaponLabel.dst.w / 2;
    float weaponStatY = weaponDst.y - weaponLabel.dst.h +160;

    weaponStatEntity.addComponent<Label>(weaponStat);
    weaponStatEntity.addComponent<Transform>(Vector2D(weaponStatX, weaponStatY), 0.0f, 1.0f);

    weaponStatEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&weaponStatEntity);

    //Stat Label


    // Clickable
    auto& weaponClickable = weaponButton.addComponent<Clickable>();

    weaponClickable.onPressed = [&weaponTransform]() {
        weaponTransform.scale = 0.9f;
    };

    weaponClickable.onReleased = [this, &overlay, w, i, &weaponTransform]() {
        weaponTransform.scale = 1.0f;

        world.getEventManager().emit(LevelUpChoiceEvent{true, w, i});

        toggleSettingsOverlayVisibility(overlay);
        createInventoryUI(800, 600); // use design size
    };

    weaponClickable.onCancel = [&weaponTransform]() {
        weaponTransform.scale = 1.0f;
    };

    weaponButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&weaponButton);

    // =========================================================
    // ITEM BUTTON
    // =========================================================
    auto& itemButton = world.createEntity();

    auto& itemTransform = itemButton.addComponent<Transform>(
        Vector2D(startX + BUTTON_SIZE + SPACING, centerY), 0.0f, 1.0f
    );

    SDL_Texture* itemTex = TextureManager::load(i.path.c_str());

    SDL_FRect itemDst = {
        itemTransform.position.x,
        itemTransform.position.y,
        BUTTON_SIZE,
        BUTTON_SIZE
    };

    SDL_FRect itemSrc = {0, 0, 32, 32};

    itemButton.addComponent<Sprite>(itemTex, itemSrc, itemDst, RenderLayer::UI, true);
    itemButton.addComponent<Collider>("ui", itemDst);

    // Label
    auto& itemLabelEntity = world.createEntity();

    Label itemLabel = {
        i.name,
        AssetManager::getFont("bungee"),
        {0,0,0,255},
        LabelType::UI,
        "itemLabel"
    };

    itemLabel.textureCacheKey = "item_" + i.name;
    itemLabel.dirty = true;

    TextureManager::loadLabel(itemLabel);

    float itemLabelX = itemDst.x + BUTTON_SIZE / 2 - itemLabel.dst.w / 2;
    float itemLabelY = itemDst.y - itemLabel.dst.h - 10;

    itemLabelEntity.addComponent<Label>(itemLabel);
    itemLabelEntity.addComponent<Transform>(Vector2D(itemLabelX, itemLabelY), 0.0f, 1.0f);

    itemLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&itemLabelEntity);

    //Item Stat Label
    auto& itemStatEntity = world.createEntity();

    Label itemStat = {
        "+" + i.statName,
        AssetManager::getFont("bungeeSmall"),
        {0,100,0,255},
        LabelType::UI,
        "itemStat"
    };
    itemStat.dirty = true;

    TextureManager::loadLabel(itemStat);

    float itemStatX = itemDst.x + BUTTON_SIZE / 2 - itemStat.dst.w / 2;
    float itemStatY = itemDst.y - itemStat.dst.h +160;

    itemStatEntity.addComponent<Label>(itemStat);
    itemStatEntity.addComponent<Transform>(Vector2D(itemStatX, itemStatY), 0.0f, 1.0f);

    itemStatEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&itemStatEntity);


    // Clickable
    auto& itemClickable = itemButton.addComponent<Clickable>();

    itemClickable.onPressed = [&itemTransform]() {
        itemTransform.scale = 0.9f;
    };

    itemClickable.onReleased = [this, &overlay, w, i, &itemTransform]() {
        itemTransform.scale = 1.0f;

        world.getEventManager().emit(LevelUpChoiceEvent{false, w, i});

        toggleSettingsOverlayVisibility(overlay);
        createInventoryUI(800, 600);
    };

    itemClickable.onCancel = [&itemTransform]() {
        itemTransform.scale = 1.0f;
    };

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
    float rowSpacing = 60.0f;
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
        SDL_FRect dst = {transform.position.x, transform.position.y, 48, 48};

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
        SDL_FRect dst = {transform.position.x, transform.position.y, 48, 48 };

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

Entity & Scene::createPointsLabel() {
    auto& pointsLabel(world.createEntity());
    Label label = {
        "Test String",
        AssetManager::getFont("bungeeLarge"),
        {255,255,255,255},
        LabelType::PlayerPoints,
        "playerPoints"
    };
    TextureManager::loadLabel(label);
    pointsLabel.addComponent<Label>(label);
    pointsLabel.addComponent<Transform>(Vector2D(10.0f, 30.0f), 0.0f, 1.0f);
    return pointsLabel;
}

void Scene::createLevelUpUI(int windowWidth, int windowHeight) {
    //create backing
    auto& back = world.createEntity();
    back.addComponent<LevelUpBar>();
    auto& c = back.addComponent<Children>();
    auto& t = back.addComponent<Transform>(Vector2D(0.0f, 0.0f), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/colors.png");

    SDL_FRect src{0,64,32,32};
    SDL_FRect dst{t.position.x, t.position.y, windowWidth * 1.0f, 25.0f};

    back.addComponent<Sprite>(tex, src, dst, RenderLayer::UI, true);

    //create levelup bar
    float offset = 2.5f;
    auto& l = world.createEntity();
    l.addComponent<Parent>(&back);
    back.getComponent<Children>().children.push_back(&l);
    auto& lt = l.addComponent<Transform>(Vector2D(t.position.x + offset, t.position.y + offset), 0.0f, 1.0f);

    SDL_Texture* ltex = TextureManager::load("../assets/colors.png");

    SDL_FRect lsrc{65,33,30,31};
    SDL_FRect ldst{lt.position.x, lt.position.y, 0.0f, dst.h - offset * 2};

    l.addComponent<Sprite>(ltex, lsrc, ldst, RenderLayer::UI, true);
}

void Scene::createHealthBar(int windowWidth, int windowHeight) {
    auto& back = world.createEntity();
    back.addComponent<HealthBar>();
    auto& c = back.addComponent<Children>();
    auto& t = back.addComponent<Transform>(Vector2D(windowWidth - 225.0f, windowHeight - 50.0f), 0.0f, 1.0f);

    SDL_Texture* tex = TextureManager::load("../assets/colors.png");

    SDL_FRect src{0,64,32,32};
    SDL_FRect dst{t.position.x, t.position.y, 200.0f, 25.0f};

    back.addComponent<Sprite>(tex, src, dst, RenderLayer::UI, true);

    //create health bar
    float offset = 2.5f;
    auto& health = world.createEntity();
    health.addComponent<Parent>(&back);
    c.children.push_back(&health);
    auto& ht = health.addComponent<Transform>(Vector2D(t.position.x + offset, t.position.y + offset), 0.0f, 1.0f);

    SDL_Texture* htex = TextureManager::load("../assets/colors.png");

    SDL_FRect hsrc{0,0,32,32};
    SDL_FRect hdst{t.position.x, t.position.y, 200.0f - offset * 2, 25.0f - offset * 2};

    health.addComponent<Sprite>(htex, hsrc, hdst, RenderLayer::UI, true);

}
