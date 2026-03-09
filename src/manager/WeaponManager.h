//
// Created by BCIT on 2026-03-08.
//

#ifndef INC_8051TUTORIAL_WEAPONMANAGER_H
#define INC_8051TUTORIAL_WEAPONMANAGER_H
#include "Component.h"

class WeaponManager {
	static std::unordered_map<std::string,Weapon > weapons;
	static Weapon loadWeaponFromXML(const char* path);

public:
	static void loadWeapon(const std::string& weaponName, const char* path);
	static const Weapon& getRandWeapon();
};

#endif //INC_8051TUTORIAL_WEAPONMANAGER_H