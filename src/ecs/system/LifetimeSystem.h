//
// Created by BCIT on 2026-03-26.
//

#ifndef INC_8051TUTORIAL_LIFETIMESYSTEM_H
#define INC_8051TUTORIAL_LIFETIMESYSTEM_H
#include <vector>
#include <memory>

#include "Component.h"

// system is so weapons like Ring of Fire that do not expire can expire without
// exceeding bounds of camera

class Entity;
class World;

class LifetimeSystem {
public:
	void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
		    	for (auto& e: entities) {
	  	if (e->hasComponent<Lifetime>()) {
			auto& lifetime = e->getComponent<Lifetime>();

	  		if (lifetime.justSpawned) {
	  			lifetime.justSpawned = false;
	  			continue;
	  		}
			float oldTime = lifetime.time;
			lifetime.time -= dt;

			if (lifetime.time <= 0) {

		    	e->destroy();

			}
	  	}
    	}
	}
};

#endif //INC_8051TUTORIAL_LIFETIMESYSTEM_H