//
// Created by beckh on 2026-03-11.
//

#ifndef PLAYERSTATLISTENER_H
#define PLAYERSTATLISTENER_H
#include <event/BaseEvent.h>


class World;

class PlayerStatListener {
    public:
    World& world;
    PlayerStatListener(World& world);

private:
    void applyChanges(const CalculateStatsEvent& event);
};

#endif //PLAYERSTATLISTENER_H
