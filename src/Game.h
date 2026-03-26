//
// Created by Weckest on 2026-01-07.
//

// #ifndef INC_8051TUTORIAL_GAME_H
// #define INC_8051TUTORIAL_GAME_H
//
// #endif //INC_8051TUTORIAL_GAME_H

#pragma once
#include <chrono>
#include <SDL3/SDL.h>

#include "World.h"
#include "manager/AudioManager.h"
#include "manager/SceneManager.h"

struct GameState {
    int playerHealth;
};

class Game {
public:
    Game();
    ~Game();

    //these are the types that SDL uses
    void init(const char* title, int width, int height, bool fullscreen);

    //game loop functons (handleEvents, update, render)
    void handleEvents(); //checks for inputs and system events
    void update(float deltaTime); //handles the game logic and changes to game state
    void render(); //handles the drawing of the current game state to the screen
    SDL_Window* getWindow() {
        return window;
    }

    //used to free resources
    void destroy();

    bool running() {
        return isRunning;
    }


    SDL_Renderer* renderer = nullptr;

    AudioManager audioManager;
    SceneManager sceneManager;
    static std::function<void(std::string)> onSceneChangeRequest;
    static GameState gameState;

private:
    int frameCount = 0;
    bool isRunning = false;


    SDL_Window* window = nullptr;
    SDL_Event event;

    //sdl stores color channels as 8-bit unsigned
    //range is 0-255
    Uint8 r, g, b, a;



};
