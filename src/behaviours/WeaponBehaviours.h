//
// Created by BCIT on 2026-03-08.
//

#ifndef INC_8051TUTORIAL_WEAPONBEHAVIOURS_H
#define INC_8051TUTORIAL_WEAPONBEHAVIOURS_H

#pragma once
#include <functional>

#include "Component.h"
#include "Vector2D.h"

class World;

extern std::unordered_map<std::string, std::function<void(Weapon&, Entity&, World&)>> weaponBehaviours;

std::function<void(Weapon&, Entity&, World&)> getWeaponBehaviour(const std::string& name);

#endif //INC_8051TUTORIAL_WEAPONBEHAVIOURS_H