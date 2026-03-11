//
// Created by Weckest on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_STAT_H
#define INC_8051TUTORIAL_STAT_H
#include <string>


struct Stat {
    std::string name;
    float defaultValue;
    float currentValue;
    float upgradeRate;
};


#endif //INC_8051TUTORIAL_STAT_H