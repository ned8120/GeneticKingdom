#include "ResourceSystem.h"

ResourceSystem::ResourceSystem(int initialGold) : gold(initialGold) {
}

int ResourceSystem::getGold() const {
    return gold;
}

void ResourceSystem::addGold(int amount) {
    if (amount > 0) {
        gold += amount;
    }
}

bool ResourceSystem::spendGold(int amount) {
    if (amount > 0 && gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}