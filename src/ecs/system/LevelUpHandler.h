//
// Created by beckh on 2026-03-11.
//

#ifndef LEVELUPHANDLER_H
#define LEVELUPHANDLER_H
#include <event/BaseEvent.h>


class World;

class LevelUpHandler {

public:
    World& world;
    LevelUpHandler(World& world);

private:
    void onLevelUp(const LevelUpEvent& event);
    void itemAdd(const Item& item);
    void weaponAdd(const dataBundle& bundle);
    void onLevelUpChoice(const LevelUpChoiceEvent& event);


};


#endif //LEVELUPHANDLER_H
