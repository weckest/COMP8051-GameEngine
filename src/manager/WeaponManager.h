//
// Created by BCIT on 2026-03-08.
//

#ifndef INC_8051TUTORIAL_WEAPONMANAGER_H
#define INC_8051TUTORIAL_WEAPONMANAGER_H
#include "Component.h"

class WeaponManager {
	static std::vector<Weapon> weapons;

public:
	static void loadWeaponFromXML(const char* path);
	static const dataBundle getRandWeapon();
	 void switchWeapon(Entity& entity);
	static const Weapon getWeapon(std::string name);
	static void upgradeRandStat(Entity &entity, dataBundle bundle);
};

#endif //INC_8051TUTORIAL_WEAPONMANAGER_H