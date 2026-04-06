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

    pClick.onReleased = [this, &playSprite, playNormal] {
        playSprite.texture = playNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
        Game::onSceneChangeRequest("gameplay");
    };

    auto& playText = world.createEntity();
    playText.addComponent<Transform>(Vector2D(playButton.getComponent<Transform>().position.x + 34, windowHeight / 2.0f + 1), 0.0f, 1.0f);

    Label plLabel = {
        "PLAY",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmPlayLabel"
    };
    auto& playLabel = playText.addComponent<Label>(plLabel);
    playLabel.dirty = true;

    //SETTINGS
    auto& setButton = makeGenericButton("grey", windowHeight / 2.0f + 70, windowWidth);
    auto& setSprite = setButton.getComponent<Sprite>();
    SDL_Texture *setNormal = setSprite.texture;
    auto& setClick = setButton.getComponent<Clickable>();

    auto& setText = world.createEntity();
    setText.addComponent<Transform>(Vector2D(setButton.getComponent<Transform>().position.x + 6, windowHeight / 2.0f + 71), 0.0f, 1.0f);

    Label seLabel = {
        "SETTINGS",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmSetLabel"
    };
    auto& settingsLabel = setText.addComponent<Label>(seLabel);
    settingsLabel.dirty = true;

    //CREDITS
    auto& credButton = makeGenericButton("grey", windowHeight / 2.0f + 140, windowWidth);
    auto& credSprite = credButton.getComponent<Sprite>();
    SDL_Texture *credNormal = credSprite.texture;
    auto& credClick = credButton.getComponent<Clickable>();


    auto& credText = world.createEntity();
    credText.addComponent<Transform>(Vector2D(credButton.getComponent<Transform>().position.x + 12, windowHeight / 2.0f + 141), 0.0f, 1.0f);

    Label crLabel = {
        "CREDITS",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmCredLabel"
    };
    auto& creditsLabel = credText.addComponent<Label>(crLabel);
    creditsLabel.dirty = true;

    //QUIT
    auto& quitButton = makeGenericButton("red", windowHeight / 2.0f + 210, windowWidth);
    auto& quitSprite = quitButton.getComponent<Sprite>();
    SDL_Texture *quitNormal = quitSprite.texture;
    auto& quitClick = quitButton.getComponent<Clickable>();

    quitClick.onReleased = [this, &quitSprite, quitNormal] {
        quitSprite.texture = quitNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
        Game::onSceneChangeRequest("quit");
    };

    auto& quitText = world.createEntity();
    quitText.addComponent<Transform>(Vector2D(quitButton.getComponent<Transform>().position.x + 34, windowHeight / 2.0f + 211), 0.0f, 1.0f);

    Label quLabel = {
        "QUIT",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmQuitLabel"
    };
    auto& quitLabel = quitText.addComponent<Label>(quLabel);
    quitLabel.dirty = true;

    //OVERLAYS
    auto& setBox = createSettingsBox(windowWidth, windowHeight);
    //TODO: UPDATE TO CRED BOX WHEN WORKING ON THIS
    auto& credBox = createCreditsBox(windowWidth, windowHeight);

    setClick.onReleased = [this, &setSprite, setNormal, &setBox] {
        setSprite.texture = setNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
        toggleSettingsOverlayVisibility(setBox);
    };

    credClick.onReleased = [this, &credSprite, credNormal, &credBox] {
        credSprite.texture = credNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
        toggleSettingsOverlayVisibility(credBox);
    };

    //COLLIDER HANDLING
    menuButtonColliders.push_back(&playButton.getComponent<Collider>());
    menuButtonColliders.push_back(&setButton.getComponent<Collider>());
    menuButtonColliders.push_back(&credButton.getComponent<Collider>());
    menuButtonColliders.push_back(&quitButton.getComponent<Collider>());
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
    titleText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 120, windowHeight / 7.0f), 0.0f, 1.0f);
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
    stat1Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 151, windowHeight / 3.0f), 0.0f, 1.0f);

    int totalSeconds = static_cast<int>(Game::gameState.time);
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
    stat2Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 136, windowHeight / 3.0f + 50), 0.0f, 1.0f);

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

    //sub damages
    if (Game::gameState.WeaponDamage["bubblegun"] > 0) {
        auto& bubbleText = world.createEntity();
        bubbleText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 91, windowHeight / 3.0f + 95), 0.0f, 1.0f);

        Label bubLabel = {
            "Bubble Gun: " + std::format("{:.2f}", Game::gameState.WeaponDamage["bubblegun"]),
            AssetManager::getFont("monogram"),
            {255,255,255,255},
            LabelType::UI,
            "gameoverbubble"
        };
        auto& bubbleLabel = bubbleText.addComponent<Label>(bubLabel);
        bubbleLabel.dirty = true;
        TextureManager::loadLabel(bubbleLabel);
    }

    if (Game::gameState.WeaponDamage["shotgun"] > 0) {
        auto& shotgunText = world.createEntity();
        shotgunText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 67, windowHeight / 3.0f + 120), 0.0f, 1.0f);

        Label shotLabel = {
            "Shotgun: " + std::format("{:.2f}", Game::gameState.WeaponDamage["shotgun"]),
            AssetManager::getFont("monogram"),
            {255,255,255,255},
            LabelType::UI,
            "gameovershotgun"
        };
        auto& shotgunLabel = shotgunText.addComponent<Label>(shotLabel);
        shotgunLabel.dirty = true;
        TextureManager::loadLabel(shotgunLabel);
    }

    if (Game::gameState.WeaponDamage["RingofFire"] > 0) {
        auto& rofText = world.createEntity();
        rofText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 107, windowHeight / 3.0f + 145), 0.0f, 1.0f);

        Label fireLabel = {
            "Ring of Fire: " + std::format("{:.2f}", Game::gameState.WeaponDamage["RingofFire"]),
            AssetManager::getFont("monogram"),
            {255,255,255,255},
            LabelType::UI,
            "gameoverrof"
        };
        auto& rofLabel = rofText.addComponent<Label>(fireLabel);
        rofLabel.dirty = true;
        TextureManager::loadLabel(rofLabel);
    }


    //Stat 3
    auto& stat3Text = world.createEntity();
    stat3Text.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 121, windowHeight / 3.0f + 170), 0.0f, 1.0f);
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
    continueText.addComponent<Transform>(Vector2D(windowWidth / 2.0f - 305, windowHeight / 5.0f * 4.0f), 0.0f, 1.0f);
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

Entity& Scene::makeGenericSlider(Entity& overlay, int type, float sliderX, float sliderY)
{
    //SLIDER
    auto& slider = world.createEntity();
    auto& sTransform = slider.addComponent<Transform>(Vector2D(sliderX, sliderY), 0.0f, 1.0f);

    SDL_Texture* barTex = TextureManager::load("../assets/ui/sliderbar.png");
    SDL_FRect barSrc = {0, 0, 38, 8};
    SDL_FRect barDst = {sTransform.position.x, sTransform.position.y, 100.0f, 20.0f};

    slider.addComponent<Sprite>(barTex, barSrc, barDst, RenderLayer::UI, false);
    slider.addComponent<Collider>("ui", barDst, false);
    slider.addComponent<Clickable>();

    auto& s = slider.addComponent<Slider>();

    s.value = AudioManager::getVolume(type);
    s.onValueChanged = [this, type](float value)
    {
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("update", type, value));
    };

    auto& knob = world.createEntity();
    knob.addComponent<Transform>(Vector2D(sTransform.position.x, sTransform.position.y), 0.0f, 1.0f);
    SDL_Texture* knobTex = TextureManager::load("../assets/ui/sliderknob.png");

    SDL_FRect kSrc{0,0,10,10};
    SDL_FRect kDst{sTransform.position.x, sTransform.position.y, kSrc.w * 2.0f, kSrc.h * 2.0f};

    knob.addComponent<Sprite>(knobTex, kSrc, kDst, RenderLayer::UI, false);
    knob.addComponent<Clickable>();
    knob.addComponent<Collider>("ui", kDst, false);
    knob.addComponent<SliderKnob>(SliderKnob{&slider});

    slider.addComponent<Parent>(&overlay);
    knob.addComponent<Parent>(&overlay);

    auto& overlayChildren = overlay.getComponent<Children>();
    overlayChildren.children.push_back(&slider);
    overlayChildren.children.push_back(&knob);

    return slider;
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
    auto& overlayChildren = overlay.getComponent<Children>();

    //exit
    auto& setExitButton = makeGenericButton(
        "red", overlaySprite.dst.y + 350, overlaySprite.dst.x + overlaySprite.dst.w + 160);
    auto& setExitSprite = setExitButton.getComponent<Sprite>();
    setExitSprite.visible = false;
    SDL_Texture *setExitNormal = setExitSprite.texture;
    auto& setExitClick = setExitButton.getComponent<Clickable>();

    setExitClick.onReleased = [this, &setExitSprite, setExitNormal, &overlay] {
        setExitSprite.texture = setExitNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));
        toggleSettingsOverlayVisibility(overlay);
    };

    auto& setExitText = world.createEntity();
    setExitText.addComponent<Transform>(
        Vector2D(overlaySprite.dst.x + overlaySprite.dst.w / 2.0f - 25, overlaySprite.dst.y + 351), 0.0f, 1.0f);

    Label sExitLabel = {
        "BACK",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmsExit"
    };
    auto& setExitLabel = setExitText.addComponent<Label>(sExitLabel);

    setExitLabel.visible = false;
    setExitLabel.dirty = true;

    setExitButton.addComponent<Parent>(&overlay);
    setExitText.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&setExitButton);
    overlayChildren.children.push_back(&setExitText);

    auto& setTitleEntity = world.createEntity();
    auto& setTitleLabel = setTitleEntity.addComponent<Label>(Label{
        "SETTINGS",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmsTitle"
    });

    setTitleEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 100, overlayTrans.position.y + 40), 0.0f, 1.0f);

    setTitleEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&setTitleEntity);

    setTitleLabel.visible = false;
    setTitleLabel.dirty = true;

    //music
    auto& musicEntity = world.createEntity();
    auto& musicLabel = musicEntity.addComponent<Label>(Label{
        "MUSIC",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmsMusic"
    });

    musicEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 5.0f + 7, overlaySprite.dst.y + 150), 0.0f, 1.0f);


    musicEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&musicEntity);

    musicLabel.visible = false;
    musicLabel.dirty = true;

    makeGenericSlider(overlay, 0, overlayTrans.position.x + overlaySprite.dst.w / 5.0f - 10, overlaySprite.dst.y + 180);

    //general
    auto& genEntity = world.createEntity();
    auto& genLabel = genEntity.addComponent<Label>(Label{
        "GENERAL",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmsGen"
    });

    genEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 5.0f * 3.0f + 13, overlaySprite.dst.y + 150), 0.0f, 1.0f);

    genEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&genEntity);

    genLabel.visible = false;
    genLabel.dirty = true;

    makeGenericSlider(overlay, 1, overlayTrans.position.x + overlaySprite.dst.w / 5.0f * 3.0f + 10, overlaySprite.dst.y + 180);

    //weapons
    auto& wepEntity = world.createEntity();
    auto& wepLabel = wepEntity.addComponent<Label>(Label{
        "WEAPONS",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmsWeap"
    });

    wepEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 5.0f - 7, overlaySprite.dst.y + 250), 0.0f, 1.0f);

    wepEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&wepEntity);

    wepLabel.visible = false;
    wepLabel.dirty = true;

    makeGenericSlider(overlay, 2, overlayTrans.position.x + overlaySprite.dst.w / 5.0f - 10, overlaySprite.dst.y + 280);

    //damage
    auto& dmgEntity = world.createEntity();
    auto& dmgLabel = dmgEntity.addComponent<Label>(Label{
        "DAMAGE",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmsDmg"
    });

    dmgEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 5.0f * 3.0f + 19, overlaySprite.dst.y + 250), 0.0f, 1.0f);

    dmgEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&dmgEntity);

    dmgLabel.visible = false;
    dmgLabel.dirty = true;

    makeGenericSlider(overlay, 3, overlayTrans.position.x + overlaySprite.dst.w / 5.0f * 3.0f + 10, overlaySprite.dst.y + 280);


}

Entity& Scene::createCreditsBox(int windowWidth, int windowHeight) {
    auto& overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../assets/ui/TFC-MenuBox.png");

    SDL_FRect overlaySrc = {0,0,windowWidth*0.60f,windowHeight*0.80f};
    SDL_FRect overlayDst = {
        (float)windowWidth/2 - overlaySrc.w/2,
        (float)windowHeight/2 - overlaySrc.h / 2,
        overlaySrc.w,
        overlaySrc.h};

    overlay.addComponent<Transform>(Vector2D(overlayDst.x, overlayDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDst, RenderLayer::UI, false);

    createCreditsComponents(overlay);

    return overlay;
}

void Scene::createCreditsComponents(Entity &overlay) {
    if (!overlay.hasComponent<Children>() ) {
        overlay.addComponent<Children>();
    }

    auto& overlayTrans = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();
    auto& overlayChildren = overlay.getComponent<Children>();

    auto& credExitButton = makeGenericButton(
        "red", overlaySprite.dst.y + 350, overlaySprite.dst.x + overlaySprite.dst.w + 160);
    auto& credExitSprite = credExitButton.getComponent<Sprite>();
    credExitSprite.visible = false;
    SDL_Texture *credExitNormal = credExitSprite.texture;
    auto& credExitClick = credExitButton.getComponent<Clickable>();

    credExitClick.onReleased = [this, &credExitSprite, credExitNormal, &overlay] {
        toggleSettingsOverlayVisibility(overlay);
        credExitSprite.texture = credExitNormal;
        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 1, 0.0f));

    };

    auto& credExitText = world.createEntity();
    credExitText.addComponent<Transform>(
        Vector2D(overlaySprite.dst.x + overlaySprite.dst.w / 2.0f - 25, overlaySprite.dst.y + 351), 0.0f, 1.0f);

    Label cExitLabel = {
        "BACK",
        AssetManager::getFont("monogram-button"),
        {255,255,255,255},
        LabelType::UI,
        "mmcExit"
    };
    auto& credExitLabel = credExitText.addComponent<Label>(cExitLabel);

    credExitLabel.visible = false;
    credExitLabel.dirty = true;

    credExitButton.addComponent<Parent>(&overlay);
    credExitText.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&credExitButton);
    overlayChildren.children.push_back(&credExitText);


    //title
    auto& credTitleEntity = world.createEntity();
    auto& credTitleLabel = credTitleEntity.addComponent<Label>(Label{
        "CREDITS",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmcTitle"
    });

    credTitleEntity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 90, overlayTrans.position.y + 40), 0.0f, 1.0f);

    credTitleEntity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&credTitleEntity);

    credTitleLabel.visible = false;
    credTitleLabel.dirty = true;

    //name1
    auto& name1Entity = world.createEntity();
    auto& name1Label = name1Entity.addComponent<Label>(Label{
        "Weckest",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmcn1"
    });

    name1Entity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 90, overlayTrans.position.y + 110), 0.0f, 1.0f);

    name1Entity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&name1Entity);

    name1Label.visible = false;
    name1Label.dirty = true;

    //name2
    auto& name2Entity = world.createEntity();
    auto& name2Label = name2Entity.addComponent<Label>(Label{
        "HeckB",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmcn2"
    });

    name2Entity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 62, overlayTrans.position.y + 160), 0.0f, 1.0f);

    name2Entity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&name2Entity);

    name2Label.visible = false;
    name2Label.dirty = true;

    //name3
    auto& name3Entity = world.createEntity();
    auto& name3Label = name3Entity.addComponent<Label>(Label{
        "Flanks",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmcn3"
    });

    name3Entity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 78, overlayTrans.position.y + 210), 0.0f, 1.0f);

    name3Entity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&name3Entity);

    name3Label.visible = false;
    name3Label.dirty = true;

    //name4
    auto& name4Entity = world.createEntity();
    auto& name4Label = name4Entity.addComponent<Label>(Label{
        "Zynity",
        AssetManager::getFont("monogram-title"),
        {255,255,255,255},
        LabelType::UI,
        "mmcn4"
    });

    name4Entity.addComponent<Transform>(Vector2D(
        overlayTrans.position.x + overlaySprite.dst.w / 2 - 78, overlayTrans.position.y + 260), 0.0f, 1.0f);

    name4Entity.addComponent<Parent>(&overlay);
    overlayChildren.children.push_back(&name4Entity);

    name4Label.visible = false;
    name4Label.dirty = true;

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
    }


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

    //add entities
    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.x = 0;
    camView.y = 0;
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, world.getMap().width * 32.0f, world.getMap().height * 32.0f);


    auto& player = world.createEntity();
    world.setPlayer(&player);
    player.addComponent<PlayerTag>();
    auto& playerStats = player.addComponent<Stats>();

    //direct position of middle of intersection
    auto& playerTransform = player.addComponent<Transform>(Vector2D((14 * 32) - 16,(25 * 32) + 16), 0.0f, 1.0f);

    player.addComponent<Velocity>(Vector2D(0.0f,0.0f), 120.0f * playerStats.speedModifier);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../assets/animations/player_anim.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
    SDL_FRect playerDst = {playerTransform.position.x, playerTransform.position.y, 64 * playerStats.playerSizeModifier, 64 * playerStats.playerSizeModifier};
    player.addComponent<Sprite>(tex, playerSrc, playerDst, RenderLayer::World);

    // Reduce hitbox size of the player to better reflect sprite
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

    player.getComponent<WeaponList>().weapons.push_back(WeaponManager::getWeapon("shotgun"));


    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{830, 120}});
    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{1180, 990}});
    world.getEventManager().emit(SpawnPrefabEvent{"magnet", Vector2D{1210, 280}});
    world.getEventManager().emit(SpawnPrefabEvent{"food", Vector2D{190, 120}});

    //add scene state
    auto& state(world.createEntity());
    state.addComponent<SceneState>();

    createPointsLabel();
    world.initDebug();
    createInventoryUI(windowWidth, windowHeight);
    createLevelUpUI(windowWidth, windowHeight);
    createHealthBar(windowWidth, windowHeight);

}

void Scene::toggleSettingsOverlayVisibility(Entity& overlay) {

    auto& sprite = overlay.getComponent<Sprite>();

    bool newVisibility = !sprite.visible;

    sprite.visible = newVisibility;
    toggleColliders(newVisibility);

    if (overlay.hasComponent<Children>()) {

       auto& children = overlay.getComponent<Children>();

        for (auto& child : children.children) {

            if (child && child->hasComponent<Collider>()) {
                child->getComponent<Collider>().enabled = newVisibility;
            }

            if (child && child->hasComponent<Sprite>()) {

                child->getComponent<Sprite>().visible = newVisibility;

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

        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 3, 0.0f));

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

        world.getAudioEventQueue().push(std::make_unique<AudioEvent>("select", 3, 0.0f));

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
