//
// Created by beckh on 2026-03-09.
//

#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP
#include <Component.h>


class ItemManager {
    static std::unordered_map<std::string,Item > items;

public:
    static const Item& getRandItem();
    static void loadAllItems(const char* path);

};
#endif //ITEMMANAGER_HPP
