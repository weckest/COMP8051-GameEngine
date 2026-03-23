//
// Created by Weckest on 2026-03-10.
//

#include "Timer.h"

#include <iostream>
#include <ostream>

#include "SDL3/SDL.h"

float Timer::nstoms(float ns) {
    return ns * 1e-6f;
}

Timer::Timer() {
    timers.reserve(10);
}

void Timer::startTimer(std::string name) {
    timers.erase(name);
    timers.emplace(name, SDL_GetTicksNS());
}

float Timer::stopTimer(std::string name) {
    Uint64 startTime = timers[name];
    results[name].push_back(SDL_GetTicksNS() - startTime);
    return results[name].back();
}

Uint64 Timer::getTimer(std::string name) {
    return timers[name];
}

float Timer::getResult(std::string name) {
    return results[name].back();
}

float Timer::getAvgResult(std::string name) {
    float sum = 0;
    int count = 0;
    for (float time : results[name]) {
        sum += time;
        count++;
    }
    return sum / count;
}

void Timer::printResults() {
    std::cout << "" << std::endl;
    for (std::pair<std::string, std::vector<float>> result : results) {
        std::cout << result.first << ": " << nstoms(getResult(result.first)) << "ms, Avg: " << nstoms(getAvgResult(result.first)) << "ms" << std::endl;
    }
}
