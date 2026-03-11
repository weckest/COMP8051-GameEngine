//
// Created by beckh on 2026-03-11.
//

#ifndef LEVELUPHANDLER_H
#define LEVELUPHANDLER_H
#include <event/BaseEvent.h>


class World;

class LevelUpHandler {

public:
    LevelUpHandler(World& world);

private:
    void onLevelUp(const LevelUpEvent& event);


};


#endif //LEVELUPHANDLER_H
