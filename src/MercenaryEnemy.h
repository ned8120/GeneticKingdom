#ifndef MERCENARY_ENEMY_H
#define MERCENARY_ENEMY_H

#include "Enemy.h"

class MercenaryEnemy : public Enemy {
private:
    static const int BASE_HEALTH = 100;  // Vida moderada
    static const int BASE_GOLD = 30;     // Muy alto valor en oro

public:
    MercenaryEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex);
    
    std::string getType() const override { return "Mercenario"; }
};

#endif // MERCENARY_ENEMY_H