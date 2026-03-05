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


class LevelUpSystem {



    public:


    void update(const std::vector<std::unique_ptr<Entity>>& entities) {

        //Get player
        Entity* playerEntity = nullptr;

        //find the player
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }
        if (!playerEntity) return;


        //Gets the stats. If xp is more than threshold, level up, subtract xp from player.
        auto& stats = playerEntity->getComponent<PlayerTag>();
        if (stats.xp >= xpToLevelUp) {
            stats.level++;
            stats.xp -= xpToLevelUp;

        }





    }

private:
    float xpToLevelUp = 100;

    //Function to increase cap.
    void levelUp() {
        xpToLevelUp += 100;
        std::cout << "Level Up" << std::endl;

        //Do emitting stuff and getters for weapon and item, to send to the level up menu



    }



};

#endif //LEVELUPSYSTEM_HPP
