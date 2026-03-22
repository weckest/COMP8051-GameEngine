//
// Created by BCIT on 2026-03-08.
//

#include "WeaponManager.h"

#include <iostream>

#include "./WeaponBehaviours.h"

#include "tinyxml2.h"

std::vector<Weapon> WeaponManager::weapons;


const Weapon& WeaponManager::getRandWeapon() {

	if (weapons.empty()) {
		throw std::runtime_error("No weapons loaded");
	}
	int randIndex = rand() % weapons.size();
	return weapons[randIndex];
}

void WeaponManager::loadWeaponFromXML(const char *path) {

	tinyxml2::XMLDocument doc;
	doc.LoadFile(path);

	auto* weaponsRoot = doc.FirstChildElement("weapons");

	for (auto* root = weaponsRoot->FirstChildElement("weapon");
	     root != nullptr;
	     root = root->NextSiblingElement("weapon")) {

		Weapon weapon;

		auto* nameAttr = root->Attribute("name");
		if (nameAttr) {
			weapon.name = nameAttr;
		}

		std::cout<< "weapon loaded" << weapon.name << std::endl;

		root->QueryFloatAttribute("fireRate", &weapon.fireRate);
		root->QueryFloatAttribute("damageModifier", &weapon.damageModifier);
		root->QueryFloatAttribute("projectileSizeModifier", &weapon.projectileSizeModifier);
		root->QueryFloatAttribute("aoeModifier", &weapon.aoeModifier);
		root->QueryFloatAttribute("critDamageModifier", &weapon.critDamageModifier);
		root->QueryFloatAttribute("critChanceModifier", &weapon.critChanceModifier);
		root->QueryFloatAttribute("rangeModifier", &weapon.rangeModifier);
		root->QueryFloatAttribute("spreadModifier", &weapon.spreadModifier);
		root->QueryFloatAttribute("projectileModifier", &weapon.projectileModifier);
		root->QueryFloatAttribute("cooldown", &weapon.cooldown);

		weapon.spawnFunction = getWeaponBehaviour(weapon.name);


		weapons.push_back(weapon);
	     }
}

 void WeaponManager::switchWeapon(Entity& entity) {
	//get the weapon component of the entity
	auto& weaponComp = entity.getComponent<WeaponList>();
	auto& weaponList = weaponComp.weapons;

	if (!weaponList.empty()) {
		//find out what weapon the player has

		auto& currWeapon = weaponList[0].name;

		//switch to the next weapon in the list

		auto it = std::find_if(weapons.begin(), weapons.end(), [&currWeapon](const Weapon& w) {
			return w.name == currWeapon;
		});

		if (it != weapons.end()) {

			//if we found the current weapon in the weapon manager's list, switch to the next weapon
			int index = std::distance(weapons.begin(), it);
			int nextIndex = (index + 1) % weapons.size();
			weaponList[0] = weapons[nextIndex];

		} else {
			//if we didnt find the current weapon in the weapon manager's list, switch to the first weapon
			weaponList[0] = weapons[0];
		}

	}
}