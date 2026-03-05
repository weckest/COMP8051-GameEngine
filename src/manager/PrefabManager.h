//
// Created by Weckest on 2026-03-05.
//

#ifndef INC_8051TUTORIAL_PREFABMANAGER_H
#define INC_8051TUTORIAL_PREFABMANAGER_H
#include <unordered_map>

#include "Entity.h"

class PrefabManager {
    static std::unordered_map<std::string, Entity> prefabs;
public:
    static Entity getPrefab(const std::string &tag);
    static Entity& loadPrefab(const std::string& path);
    static void addPrefab(const std::string& tag, Entity entity);
    static void clean();
private:

};

#endif //INC_8051TUTORIAL_PREFABMANAGER_H