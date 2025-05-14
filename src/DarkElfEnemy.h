#ifndef DARK_ELF_ENEMY_H
#define DARK_ELF_ENEMY_H

#include "Enemy.h"

class DarkElfEnemy : public Enemy {
private:
    static const int BASE_HEALTH = 80;  // Poca vida
    static const int BASE_GOLD = 20;    // Buen valor en oro

public:
    DarkElfEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex);
    
    std::string getType() const override { return "Elfo Oscuro"; }
};

#endif // DARK_ELF_ENEMY_H