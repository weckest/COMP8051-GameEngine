//
// Created by beckh on 2026-03-05.
//

#ifndef LEVELUPSYSTEM_HPP
#define LEVELUPSYSTEM_HPP
#include <Component.h>
#include <Entity.h>
#include <memory>
#include <vector>
#include <iostream>
#include <event/BaseEvent.h>




class LevelUpSystem {

    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, World& world);


private:
    float xpToLevelUp = 100;

};

#endif //LEVELUPSYSTEM_HPP
