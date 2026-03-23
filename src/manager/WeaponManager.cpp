//
// Created by BCIT on 2026-03-08.
//

#include "WeaponManager.h"

#include <iostream>
#include <random>

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

		auto* pathAttr = root->Attribute("path");
		if (pathAttr) {
			weapon.path = pathAttr;
		}

		for (const tinyxml2::XMLAttribute* attr = root->FirstAttribute();
				     attr != nullptr;
				     attr = attr->Next())
		{
			std::string key = attr->Name();
			std::string valueStr = attr->Value();

			// skip name attribute
			if (key == "name" || key == "path") continue;

			try {
				float value = std::stof(valueStr);

				// give weapon stat name
				weapon.weaponStats[key] = value;
				weapon.statNames.push_back(key);

			} catch (...) {
				std::cout << "Invalid value for " << key << std::endl;
			}
		}

		std::cout<< "weapon loaded" << weapon.name << std::endl;

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

void WeaponManager::upgradeRandStat(Entity& entity, Weapon& weapon) {
	//get weapon name that needs to upgrade
	auto name = weapon.name;

	//find the weapon in the player's list (entity)
	auto& weaponList = entity.getComponent<WeaponList>().weapons;

	for (auto & i : weaponList) {

		if (i.name == name) {
			// if we find the weapon upgrade a random stat
			auto& targ = i;
			auto& statNames = targ.statNames;

			// randomly pick a stat to upgrade
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(0, statNames.size() - 1);

			int randomIndex = dist(gen);
			std::string randomStat = statNames[randomIndex];

			// upgrade the stat
			targ.weaponStats[randomStat] *= 1.1f;         // +10%
			break;
		}
	}
}