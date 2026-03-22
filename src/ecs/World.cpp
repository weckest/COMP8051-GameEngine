// //
// // Created by Weckest on 2026-02-04.
// //

#include "World.h"

World::World() {
    entityGrid = std::vector<std::vector<std::vector<Entity*>>>(
            rows,
            std::vector<std::vector<Entity*>>(cols)
    );

    getEventManager().subscribe(
       [this](const BaseEvent &e) {
           if (e.type != EventType::ToggleDebug) return;

           debugState.debug = !debugState.debug;
           std::cout << "Debug: " << debugState.debug << std::endl;

       });

    getEventManager().subscribe(
        [this](const BaseEvent& e) {
            if (e.type != EventType::KeyPress) return;
            const auto& keyPress = static_cast<const KeyPressEvent&>(e);
            if (debugState.debug) {
                switch (keyPress.keyCode) {
                    case SDLK_T:
                        debugState.timer = !debugState.timer;
                        std::cout << "Debug Timer: " << debugState.timer << std::endl;
                        break;
                    case SDLK_L:
                        debugState.level = !debugState.level;
                        std::cout << "Debug Levels: " << debugState.lines << std::endl;
                        break;
                    case SDLK_J:
                        debugState.lines = !debugState.lines;
                        std::cout << "Debug Lines: " << debugState.lines << std::endl;
                        break;
                    case SDLK_C:
                        debugState.colliders = !debugState.colliders;
                        std::cout << "Debug Colliders: " << debugState.colliders << std::endl;
                        break;
                    case SDLK_G:
                        debugState.grid = !debugState.grid;
                        std::cout << "Debug Grid: " << debugState.grid << std::endl;
                        break;
                    case SDLK_R:
                        debugState.range = !debugState.range;
                        std::cout << "Debug Range: " << debugState.range << std::endl;
                        break;
                    default:
                        break;
                }
            }

            switch (keyPress.keyCode) {
                case SDLK_P:
                    isPaused = !isPaused;
                    break;
                case SDLK_Q:
                    weaponManager.switchWeapon(*player);
                    break;
                default:
                    break;
            }
    });


}
