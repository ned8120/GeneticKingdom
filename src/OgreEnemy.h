#ifndef OGRE_ENEMY_H
#define OGRE_ENEMY_H

#include "Enemy.h"

class OgreEnemy : public Enemy {
private:
    static const int BASE_HEALTH = 150;  // Mucha vida
    static const int BASE_GOLD = 15;     // Valor en oro

public:
    OgreEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex);
    
    std::string getType() const override { return "Ogro"; }
};

#endif // OGRE_ENEMY_H