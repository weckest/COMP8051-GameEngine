//
// Created by Weckest on 2026-01-07.
//

#include "Game.h"
#include "Map.h"

#include <iostream>
#include <ostream>
#include <random>
#include <chrono>
#include <manager/WeaponManager.h>

#include "AnimationClip.h"
#include "manager/AssetManager.h"
#include "manager/ItemManager.hpp"

// GameObject* player = nullptr;

GameState Game::gameState{};
std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {
    r = 255;
    g = 255;
    b = 255;
    a = 255;

    // 2. Define the distribution (e.g., uniform integer distribution)
    int min = 1;
    int max = 255;
    std::uniform_int_distribution<int> dist(min, max);

}

Game::~Game() {
    destroy();
}

void Game::init(const char *title, int width, int height, bool fullscreen)
{

    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    //Initialize SDL library
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
        std::cout << "Subsystem initialized..." << std::endl;
        window = SDL_CreateWindow(title,width, height, flags);
        if (window) {
            std::cout << "Window created..." << std::endl;
        }

        #ifdef _WIN32
                renderer = SDL_CreateRenderer(window, "direct3d");
        #elif __APPLE__
                renderer = SDL_CreateRenderer(window, "metal");
        #else
                renderer = SDL_CreateRenderer(window, nullptr);
        #endif

        if (renderer) {
            std::cout << "Renderer created..." << std::endl;
        } else {
            std::cout << "Renderer could not be created..." << std::endl;
        }

        if (TTF_Init() != 1) {
            std::cout << "TTF_Init Failed." << std::endl;
        }

        isRunning = true;
    } else {
        isRunning = false;
    }

    //load audio
    audioManager.loadAudio("theme", "../assets/audio/switch_with_me.ogg");
    audioManager.loadAudio("collect", "../assets/audio/coin.ogg");

    //load fonts
    AssetManager::loadFont("arial", "../assets/fonts/arial.ttf", 16);

    //load assets
    AssetManager::loadAnimation("player", "../assets/animations/bunny_animations.xml");
    AssetManager::loadAnimation("enemy", "../assets/animations/fox_animations.xml");
    AssetManager::loadAnimation("explosion", "../assets/animations/explosion_animation.xml");
    WeaponManager::loadWeaponFromXML("../assets/weapons/weapon.xml");

    ItemManager::loadAllItems("../assets/items/items.xml");

    // Item item = ItemManager::getRandItem();
    // std::cout << item.name << std::endl;
    // std::cout << item.aoeModifier << std::endl;
    //
    // item = ItemManager::getRandItem();
    // std::cout << item.name << std::endl;
    // std::cout << item.aoeModifier << std::endl;

    std::cout << "Animations loaded..." << std::endl;

    //load scenes
    sceneManager.loadScene(SceneType::MainMenu, "mainmenu", nullptr, width, height);
    sceneManager.loadScene(SceneType::Gameplay, "level1", "../assets/map.tmx", width, height);
    sceneManager.loadScene(SceneType::Gameplay, "level2", "../assets/map2/map2.tmx", width, height);
    sceneManager.loadScene(SceneType::Gameplay, "gameplay", "../assets/map-tlc/TLC-MapUpdated.tmx", width, height);

    std::cout << "Scenes loaded..." << std::endl;

    //init game data/state
    gameState.playerHealth = 100;

    //start music
    audioManager.playMusic("theme");

    //start level
    sceneManager.changeSceneDeferred("mainmenu");
    
    //resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {

        //some game state happening here
        if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
            std::cout << "You Win!" << std::endl;
            isRunning = false;
            return;
        }

        if (sceneName == "gameover") {
            std::cout << "Game Over" << std::endl;
            isRunning = false;
            return;
        }

        sceneManager.changeSceneDeferred(sceneName);
    };

}

void Game::handleEvents() {

    //SDL listens to the OS for input events internally and it addes them to a queue

    //check for next event, if an event is available it will remove from the queue and store in event
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT: //usually triggered when the user closes the window
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::update(float dt) {
    sceneManager.update(dt, event);
}

void Game::render() {


    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    //every frame the renderer is cleared with the draw color
    SDL_RenderClear(renderer);

    sceneManager.render();
    //all your drawing would go here
    // player->draw();
    //display everything that was just drawn
    //draws it in memory first to a back buffer

    //swaps the back buffer to the screen
    SDL_RenderPresent(renderer);

}

void Game::destroy() {
    TextureManager::clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Game destroyed..." << std::endl;
}


