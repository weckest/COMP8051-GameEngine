//
// Created by Weckest on 2026-02-25.
//

#ifndef INC_8051TUTORIAL_SCENEMANAGER_H
#define INC_8051TUTORIAL_SCENEMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "../scene/Scene.h"

class Scene;

struct SceneParams {
    SceneType sceneType;
    const char* name;
    const char* mapPath;
    int windowWidth;
    int windowHeight;
    SDL_Window* window;
};

class SceneManager {
    std::unordered_map<std::string, SceneParams> sceneParam;
    std::string pendingScene;

    void changeScene(const std::string& name) {
        auto it = sceneParam.find(name);
        if (it != sceneParam.end()) {
            const auto& params = it->second;
            //create scene object
            currentScene = std::make_unique<Scene>(
                params.sceneType,
                params.name,
                params.mapPath,
                params.windowWidth,
                params.windowHeight,
                params.window
            );
        } else {
            std::cerr << "Scene not found: " << name << std::endl;
        }
    }


public:
    std::unique_ptr<Scene> currentScene;
    void loadScene(SceneType sceneType, const char* sceneName, const char* mapPath,
               int windowWidth, int windowHeight, SDL_Window* window) {
        sceneParam[sceneName] = {sceneType, sceneName, mapPath, windowWidth, windowHeight, window};
    }

    void changeSceneDeferred(const std::string& name) {
        pendingScene = name;
    }

    void update(const float dt, SDL_Event& e, SDL_Renderer* renderer) {
        if (currentScene) currentScene->update(dt, e, renderer); //all our world updates
        if (!pendingScene.empty()) {
            changeScene(pendingScene);
            pendingScene.clear();
        }
    }

    void render() {
        if (currentScene) currentScene->render();
    }
};

#endif //INC_8051TUTORIAL_SCENEMANAGER_H