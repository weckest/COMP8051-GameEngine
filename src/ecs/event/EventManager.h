//
// Created by Weckest on 2026-02-04.
//

#ifndef INC_8051TUTORIAL_EVENTMANAGER_H
#define INC_8051TUTORIAL_EVENTMANAGER_H
#include <functional>

#include "BaseEvent.h"
#include "../Entity.h"

//observer pattern
class EventManager {
public:
    using Handler = std::function<void(const BaseEvent&)>;

    void emit(const BaseEvent& event) const {
        for (const auto& listener: listeners) {
            listener(event);
        }
    }

    void subscribe(const Handler& callback) {
        listeners.emplace_back(callback);
    }

private:
    std::vector<Handler> listeners;


};




#endif //INC_8051TUTORIAL_EVENTMANAGER_H