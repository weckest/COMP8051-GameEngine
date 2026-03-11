//
// Created by BCIT on 2026-03-10.
//

#include "WeaponFireSystem.h"

#include "World.h"
#include "manager/WeaponManager.h"

void WeaponFireSystem::update(World &world, float dt) {

	  // or however your engine exposes it

	for (const auto& entity : world.getEntities()) {
		if (!entity->hasComponent<PlayerTag>())
			continue;

		auto& player = entity->getComponent<PlayerTag>();
		auto& weaponComp = player.weaponList;

		for (auto& weapon : weaponComp) {

			weapon.cooldown -= dt;

			if (weapon.cooldown <= 0.0f) {

				weapon.spawnFunction(weapon, *entity, world);

				weapon.cooldown = weapon.fireRate; // reset timer
			}
		}
	}
}