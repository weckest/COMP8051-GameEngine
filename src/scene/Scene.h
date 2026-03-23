//
// Created by Weckest on 2026-02-25.
//

#ifndef INC_8051TUTORIAL_SCENE_H
#define INC_8051TUTORIAL_SCENE_H
#include <string>

#include "Vector2D.h"
#include "World.h"
#include "SDL3/SDL_events.h"
#include "SceneType.h"

class Scene {
public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(const float dt, SDL_Event& e) {
        world.update(dt, e, type);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const {return name;}

private:
    std::string name;
    SceneType type;
    void createProjectile(Vector2D pos, Vector2D dir, int speed);

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    Entity& createSettingsOverlay(int windowWidth, int windowHeight);
    Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);
    void createSettingsUIComponents(Entity& overlay);
    void toggleSettingsOverlayVisibility(Entity& overlay);

    Entity& createPlayerPosLabel();

    Entity& createLevelUpMenu(int windowWidth, int windowHeight, Weapon w, Item i);
};

#endif //INC_8051TUTORIAL_SCENE_H