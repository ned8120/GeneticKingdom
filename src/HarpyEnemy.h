#ifndef HARPY_ENEMY_H
#define HARPY_ENEMY_H

#include "Enemy.h"

class HarpyEnemy : public Enemy {
private:
    static const int BASE_HEALTH = 70;  // Muy poca vida
    static const int BASE_GOLD = 25;    // Alto valor en oro

public:
    HarpyEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex);
    
    // Sobrescribir método de daño para implementar inmunidad a artillería
    int takeDamage(int damage, std::string towerType) override;
    
    std::string getType() const override { return "Harpía"; }
};

#endif // HARPY_ENEMY_H