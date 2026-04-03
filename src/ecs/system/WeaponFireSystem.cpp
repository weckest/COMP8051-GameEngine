//
// Created by BCIT on 2026-03-10.
//

#include "WeaponFireSystem.h"

#include "World.h"
#include "manager/WeaponManager.h"

void WeaponFireSystem::update(World &world, float dt) {
	for (const auto& entity : world.getEntities()) {

		if (!entity->hasComponent<PlayerTag>())
			continue;

		auto& weaponList = entity->getComponent<WeaponList>();
		auto& stats = entity->getComponent<Stats>();

		for (auto& weapon : weaponList.weapons) {

			// Decrease cooldown over time
			weapon.weaponStats["cooldown"] -= dt;

			if (weapon.weaponStats["cooldown"] <= 0.0f) {

				// Fire weapon
				// std::cout << "Weapon System" << std::endl;
				weapon.spawnFunction(weapon, *entity, world);
				// std::cout << "Weapon System Done" << std::endl;
				world.getAudioEventQueue().push(std::make_unique<AudioEvent>("fire-" + weapon.name, 2));


				float baseFireRate = weapon.weaponStats["fireRate"];
				float fireRateModifier = 1.0f + 0.05f * stats.fireRateModifier;

				// Convert to cooldown (seconds per shot)
				float cooldown = 1.0f / (baseFireRate * fireRateModifier);


				weapon.weaponStats["cooldown"] = cooldown;
			}
		}
	}
}