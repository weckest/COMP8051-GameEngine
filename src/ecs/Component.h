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
#include "Entity.h"

#include "SDL3_ttf/SDL_ttf.h"


class World;

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};
};

enum class RenderLayer {
    World,
    UI
};

//direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
    bool facingRight = true;
};

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer renderLayer = RenderLayer::World;
    bool visible = true;

    //Scaling
    bool hasBase = false;
    SDL_FRect baseDst{};
};

struct Clickable {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onCancel{};
    bool pressed = false;
};

struct Parent {
    Entity* parent = nullptr;
};

struct Children {
    std::vector<Entity*> children{};
};

enum class LabelType {
    PlayerPosition,
    Damage,
    Health,
    UI,
    Debug,
    DebugStats,
    LevelUp,
    Weapons,
    Times
};

struct Label {
    std::string text{};
    TTF_Font* font = nullptr;
    SDL_Color color{};
    LabelType type = LabelType::PlayerPosition;
    std::string textureCacheKey{};
    SDL_Texture* texture = nullptr;
    SDL_FRect dst{};
    bool visible = true;
    bool dirty = false;
};

enum CollisionLayer {
    PLAYER = 1 << 0,
    ENEMY = 1 << 1,
    WALL = 1 << 2,
    PROJECTILE = 1 << 3
};

struct Collider {
    std::string tag{};
    SDL_FRect rect{};
    bool enabled = true;
    uint32_t layer;
    uint32_t mask;
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;

    //Scale
    SDL_FRect baseRect{};
    bool hasBase = false;
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


struct EffectTag{};
struct MagnetTag{};
struct RingFireTag {
    float range;
};
struct FoodTag {
    float heal{};
};
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

struct Weapon {
    std::string name;
    std::string path;
    // float fireRate{};
    // float damageModifier{};
    // float projectileSizeModifier{};
    // float aoeModifier{};
    // float critDamageModifier{};
    // float critChanceModifier{};
    // float rangeModifier{};
    // float spreadModifier{};
    // float projectileModifier{};
    // float cooldown{};
    std::unordered_map<std::string, float> weaponStats{};
    std::vector<std::string> statNames{};
    int amtLevelUps;
    // a function that takes in the above modifiers and spawns a projectile with those modifiers applied
    //  the entity representing the player so we can get the player's transform and direction to spawn the projectile in the right place and direction
    std::function<void( Weapon&, Entity& , World&)> spawnFunction;
};

struct Item {
    std::string name;
    int itemLevel = 1;
    float damageModifier = 1;
    float speedModifier=1;
    float fireRateModifier=1;
    float playerSizeModifier=1;
    float projectileSizeModifier=1;
    float aoeModifier=1;
    float xpModifier = 1;
    std::string path = "test";
    std::string statName{};

};

struct PlayerTag {

    float xp = 0;
    int level = 1;
    int health = 100;
};

struct Stats {
    float damageModifier = 1;
    float speedModifier = 1;
    float fireRateModifier = 1;
    float playerSizeModifier = 1;
    float projectileSizeModifier = 1;
    float aoeModifier = 1;
    float xpModifier = 1;
};

struct ItemList {
    std::vector<Item> items{};
};

struct Lifetime {
    float time{};
    bool justSpawned = true;
};

struct WeaponList {
    std::vector<Weapon> weapons{};
};

struct InventoryUI{};

struct LevelUpBar{};
struct HealthBar{};

struct dataBundle {
    std::string name;
    Weapon &weapon;
};

#endif //INC_8051TUTORIAL_COMPONENT_H