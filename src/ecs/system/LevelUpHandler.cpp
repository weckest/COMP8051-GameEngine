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
}

void LevelUpHandler::onLevelUp(const LevelUpEvent& e) {

    //TODO Separate into different functions. One that takes either item or Weapon from choice.
    std::cout << "Level Up Handler Success"<< std::endl;
    Weapon w = WeaponManager::getRandWeapon();
    Item i = ItemManager::getRandItem();

    //Code for Player Choice: Connect with Menu UI

    //Something like Calling a Menu() function and passing in the weapon/item choices.


    //Gets player entity, applies item to players items.
    Entity* player = nullptr;

    // find player
    for (auto& entity : world.getEntities()) {
        if (entity->hasComponent<PlayerTag>()) {
            player = entity.get();
            break;
        }
    }

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
