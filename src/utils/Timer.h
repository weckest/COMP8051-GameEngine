//
// Created by Weckest on 2026-03-10.
//

#ifndef INC_8051TUTORIAL_TIMER_H
#define INC_8051TUTORIAL_TIMER_H
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL3/SDL_stdinc.h"

class Timer {
    std::unordered_map<std::string, Uint64> timers;
    std::unordered_map<std::string, std::vector<float>> results;

    float nstoms(float ns);
public:
    Timer();

    void startTimer(std::string name);
    float stopTimer(std::string name);
    Uint64 getTimer(std::string name);
    std::vector<std::string> getTimers();
    float getResult(std::string name);
    float getAvgResult(std::string name);
    void printResults();

};

#endif //INC_8051TUTORIAL_TIMER_H