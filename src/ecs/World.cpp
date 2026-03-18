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

       });

    getEventManager().subscribe(
        [this](const BaseEvent& e) {
            if (e.type != EventType::KeyPress) return;
            const auto& keyPress = static_cast<const KeyPressEvent&>(e);

            switch (keyPress.keyCode) {
                case SDLK_T:
                    debugState.timer = !debugState.timer;
                    break;
                case SDLK_L:
                    debugState.level = !debugState.level;
                    break;
                case SDLK_P:
                    isPaused = !isPaused;

                default:
                    break;
            }
    });


}
