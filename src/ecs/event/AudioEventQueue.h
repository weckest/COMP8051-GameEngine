//
// Created by Miguel on 2026-03-25.
//

#ifndef INC_8051TUTORIAL_AUDIOEVENTQUEUE_H
#define INC_8051TUTORIAL_AUDIOEVENTQUEUE_H
#include <memory>
#include <queue>
#include <string>

#include "manager/AudioManager.h"

class AudioEvent
{
public:
    explicit AudioEvent(const std::string &name, int type, float value) : name(name), type(type), value(value) {}

    void execute() const
    {

        if (name == "update")
        {
            std::cout << "[AudioEvent::update]" << std::endl;
            AudioManager::updateVolume(type, value);
        } else {
            switch (type)
            {
            case 1:
                AudioManager::playSfx(name);
                break;
            case 2:
                AudioManager::playWeaponSfx(name);
                break;
            case 3:
                AudioManager::playDamageSfx(name);
                break;
            default:
                std::cerr << "Unknown event type in AudioEvent" << type << std::endl;
                break;
            }
        }

    }

private:
    std::string name;
    int type;
    float value = 0.0f;
};

class AudioEventQueue
{
public:

    void push(std::unique_ptr<AudioEvent> event)
    {
        events.push(std::move(event));
    }

    void process()
    {
        while (!events.empty())
        {
            events.front()->execute();
            events.pop();
        }
    }

private:
    std::queue<std::unique_ptr<AudioEvent>> events;
};

#endif //INC_8051TUTORIAL_AUDIOEVENTQUEUE_H