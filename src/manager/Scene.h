//
// Created by Weckest on 2026-02-25.
//

#ifndef INC_8051TUTORIAL_SCENE_H
#define INC_8051TUTORIAL_SCENE_H
#include <string>

#include "Vector2D.h"
#include "World.h"
#include "SDL3/SDL_events.h"

class Scene {
public:
    Scene(const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(const float dt, SDL_Event& e) {
        world.update(dt, e);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const {return name;}

private:
    std::string name;
    void createProjectile(Vector2D pos, Vector2D dir, int speed);
};

#endif //INC_8051TUTORIAL_SCENE_H