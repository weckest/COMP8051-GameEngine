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
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath,
             int windowWidth, int windowHeight, SDL_Window* window);

    void update(const float dt, SDL_Event& e, SDL_Renderer* renderer) {
        world.update(dt, e, type, renderer);
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
    void initGameplay(SDL_Window *window, const char *mapPath, int windowWidth, int windowHeight);

    Entity& makeGenericButton(const std::string& color, int buttonHeight, int windowWidth);

    Entity& createSettingsBox(int windowWidth, int windowHeight);
    void createSettingsComponents(Entity& overlay);

    Entity& createCreditsBox(int windowWidth, int windowHeight);
    void createCreditsComponents(Entity& overlay);

    std::vector<Collider*> menuButtonColliders;
    void toggleColliders(bool isVisible);

    //LECTURE
    Entity& createSettingsOverlay(int windowWidth, int windowHeight);
    Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);
    void createSettingsUIComponents(Entity& overlay);
    void toggleSettingsOverlayVisibility(Entity& overlay);

    Entity& createPlayerPosLabel();
    Entity& createPointsLabel();

    void createLevelUpUI(int windowWidth, int windowHeight);
    void createHealthBar(int windowWidth, int windowHeight);

    Entity& createLevelUpMenu(int windowWidth, int windowHeight, dataBundle w, Item i);
    void createInventoryUI(int windowWidth, int windowHeight);
};

#endif //INC_8051TUTORIAL_SCENE_H