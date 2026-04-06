//
// Created by BCIT on 2026-03-10.
//

#include "WeaponFireSystem.h"

#include "WeaponBehaviours.h"
#include "World.h"
#include "manager/WeaponManager.h"

void WeaponFireSystem::update(World &world, float dt) {
	for (const auto& entity : world.getEntities()) {
		if (!entity->hasComponent<PlayerTag>()) continue;

		auto& weaponList = entity->getComponent<WeaponList>();
		auto& stats = entity->getComponent<Stats>();

		for (auto& weapon : weaponList.weapons) {

			// Apply fire rate upgrade from player stats
			float fireRate = WeaponManager::applyItemUpgrade(getStat(weapon, "fireRate", 1.0f), stats.fireRateModifier);

			// Decrease cooldown over time
			weapon.weaponStats["cooldown"] -= dt;

			if (weapon.weaponStats["cooldown"] <= 0.0f) {

				// Fire the weapon (lambdas handle projectiles as before)
				weapon.spawnFunction(weapon, *entity, world);
				world.getAudioEventQueue().push(std::make_unique<AudioEvent>("fire-" + weapon.name, 2, 0.0f));



				// Reset cooldown based on upgraded fire rate
				float cooldown = getStat(weapon, "baseCooldown", 1.25f) / fireRate;
				weapon.weaponStats["cooldown"] = cooldown;
			}
		}
	}
}