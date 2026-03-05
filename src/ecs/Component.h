//
// Created by Weckest on 2026-01-21.
//

#ifndef INC_8051TUTORIAL_COMPONENT_H
#define INC_8051TUTORIAL_COMPONENT_H
#include <functional>

#include "../utils/Vector2D.h"
#include "SDL3/SDL_render.h"

#include <string>
#include <unordered_map>

#include "AnimationClip.h"


struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};
};

//direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
};

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
};

struct Collider {
    std::string tag{};
    SDL_FRect rect{};
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; //time is accumulated for the current frame
    int currentFrame{}; //index of the current frame in the clip;
    float speed = 0.1f; //time per frame
};

struct Camera {
    SDL_FRect view{};
    float worldWidth;
    float worldHeight;
};

struct TimedSpawner {
    float spawnInterval;
    std::function<void()> spawnCallback;
    float timer;
};

//out game state, might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
};

struct Health {
    int currentHealth{};
};

struct PlayerTag {
    float damageModifier = 1;
    float speedModifier = 1;
    float fireRateModifier = 1;
    float playerSizeModifier = 1;
    float projectileSizeModifier = 1;
    float aoeModifier = 1;
    float xp = 0;
    int level = 1;
};
struct EffectTag{};
struct ProjectileTag {
    float damage{};
    float aoe{};
};
struct EnemyTag {
    float health{};
    float speedModifier = 1;
};
struct ItemTag {
    float time;
};

#endif //INC_8051TUTORIAL_COMPONENT_H