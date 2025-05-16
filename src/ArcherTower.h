#ifndef ARCHER_TOWER_H
#define ARCHER_TOWER_H

#include "Tower.h"

class ArcherTower : public Tower {
private:
    static const int BASE_DAMAGE = 10;      // Bajo daño
    static const int BASE_RANGE = 300;  // Para ArcherTower
    static const int BASE_ATTACK_SPEED = 500; // Ataque rápido (ms)
    static const int BASE_SPECIAL_COOLDOWN = 10000; // 10 segundos
    static const int BASE_COST = 25;        // Bajo costo
    static const int SPECIAL_ARROWS = 3;  // Número de flechas para ataque especial


public:
    ArcherTower(int r, int c, SDL_Texture* tex = nullptr);
    
    // Implementación de métodos virtuales
    void attack() override;
    std::string getType() const override { return "Arquero"; }

    void performSpecialAttack() override;

};

#endif // ARCHER_TOWER_H