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
	static const Weapon& getRandWeapon();
	 void switchWeapon(Entity& entity);
	static void upgradeRandStat(Entity& entity, Weapon& weapon);
};

#endif //INC_8051TUTORIAL_WEAPONMANAGER_H