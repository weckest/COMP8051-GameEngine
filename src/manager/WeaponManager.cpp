//
// Created by BCIT on 2026-03-08.
//

#include "WeaponManager.h"

#include "tinyxml2.h"

std::unordered_map<std::string, Weapon> WeaponManager::weapons;

void WeaponManager::loadWeapon(const std::string &weaponName, const char *path) {
	Weapon weapon = loadWeaponFromXML(path);
	weapons[weaponName] = weapon;
}

const Weapon &WeaponManager::getRandWeapon() {
	int index = rand() % weapons.size();
	auto it = weapons.begin();
	std::advance(it, index);
	return it->second;
}

Weapon WeaponManager::loadWeaponFromXML(const char *path) {
	Weapon weapon;
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path);

	auto* root = doc.FirstChildElement("weapon");
	auto* nameAttr = root->Attribute("name");

	if (nameAttr) {
		weapon.name = nameAttr;
	}

	root->QueryFloatAttribute("fireRate", &weapon.fireRate);
	root->QueryFloatAttribute("damageModifier", &weapon.damageModifier);
	root->QueryFloatAttribute("projectileSizeModifier", &weapon.projectileSizeModifier);
	root->QueryFloatAttribute("aoeModifier", &weapon.aoeModifier);
	root->QueryFloatAttribute("critDamageModifier", &weapon.critDamageModifier);
	root->QueryFloatAttribute("critChanceModifier", &weapon.critChanceModifier);

	return weapon;
}