//
// Created by Weckest on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_DEBUGSTATE_H
#define INC_8051TUTORIAL_DEBUGSTATE_H

struct DebugState {
    bool debug = false;
    bool timer = false;
    bool level = false;
    bool weapon = false;
    bool item = false;
    bool ui = false;
    bool lines = false;
    bool colliders = false;
    bool grid = false;
    bool range = false;
    bool stopSpawn = false;
    bool stopEnemies = false;
};

#endif //INC_8051TUTORIAL_DEBUGSTATE_H