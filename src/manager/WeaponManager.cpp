//
// Created by BCIT on 2026-03-08.
//

#include "WeaponManager.h"

#include <iostream>

#include "./WeaponBehaviours.h"

#include "tinyxml2.h"

std::unordered_map<std::string, Weapon> WeaponManager::weapons;


const Weapon& WeaponManager::getRandWeapon() {

	if (weapons.empty())
		throw std::runtime_error("WeaponManager: no weapons loaded");

	int index = rand() % weapons.size();
	auto it = weapons.begin();
	std::advance(it, index);
	std::cout<< it->second.name << std::endl;
	return it->second;
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


		weapons[weapon.name] = weapon;
	     }
}