//
// Created by beckh on 2026-03-11.
//

#include "LevelUpHandler.h"

#include <World.h>


//Subscribes to the Level Up event, which calls onLevelUp when emitted.
LevelUpHandler::LevelUpHandler(World &world) : world(world) {
    world.getEventManager().subscribe([this](const BaseEvent& e) {

        if (e.type!= EventType::LevelUp) {
            return;
        } else {
            const auto& lvlEvent = static_cast<const LevelUpEvent&>(e);
            onLevelUp(lvlEvent);
        }

    });

    world.getEventManager().subscribe([this](const BaseEvent& e) {

        if (e.type!= EventType::LevelUpChoice) {
            return;
        } else {
            const auto& lvlChoice = static_cast<const LevelUpChoiceEvent&>(e);
            onLevelUpChoice(lvlChoice);

        }
    });
}

void LevelUpHandler::onLevelUp(const LevelUpEvent& e) {


    world.togglePaused();
    Weapon w = WeaponManager::getRandWeapon();
    Item i = ItemManager::getRandItem();

    world.getEventManager().emit(ShowLevelUpMenuEvent{w, i});


}

void LevelUpHandler::onLevelUpChoice(const LevelUpChoiceEvent& e) {

    if (e.choseWeapon) {
        std::cout << "Chose Weapon"<< std::endl;

        weaponAdd(e.weapon);
    } else {
        std::cout << "Chose Item"<< std::endl;

        itemAdd(e.item);
    }

    world.getEventManager().emit(CalculateStatsEvent());
    world.togglePaused();
}

void LevelUpHandler::itemAdd(const Item& i) {
    Entity* player = nullptr;

    // find player
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>()) {
            player = entity.get();
            break;
        }
    }

    //Gets player entity, applies item to players items.

    auto& itemListComponent = player->getComponent<ItemList>();


    //Upgrades level of item if already exists
    for (auto& item: itemListComponent.items) {
        if (item.name == i.name) {

            item.itemLevel +=1;
            return;
        }
    }
    itemListComponent.items.push_back(i);

    //Print out all items inside.
    for (Item item : itemListComponent.items) {
        std::cout << item.name << ": " << item.itemLevel << std::endl;
    }
}


//TODO Brian add how you handle getting a weapon
void LevelUpHandler::weaponAdd(const Weapon &weapon) {

    Entity* player = nullptr;

    // find player
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>()) {
            player = entity.get();
            break;
        }
    }

    //upgrade weapon if duplicate exist
    for (auto& weapons: player->getComponent<WeaponList>().weapons) {
        if (weapons.name == weapon.name) {
            WeaponManager::upgradeRandStat(*player, weapons);
            return;
        }
    }

    // if the duplicate does not exist add it to the players pool
    player->getComponent<WeaponList>().weapons.push_back(weapon);

}

