#ifndef MAGE_TOWER_H
#define MAGE_TOWER_H

#include "Tower.h"

class MageTower : public Tower {
private:
    static const int BASE_DAMAGE = 20;       // Daño medio
    static const int BASE_RANGE = 100;       // Alcance medio
    static const int BASE_ATTACK_SPEED = 1000; // Velocidad media (ms)
    static const int BASE_SPECIAL_COOLDOWN = 8000; // 8 segundos
    static const int BASE_COST = 50;         // Costo medio
    static constexpr float AREA_DAMAGE_RADIUS = 50.0f;


public:
    MageTower(int r, int c, SDL_Texture* tex = nullptr);
    
    // Implementación de métodos virtuales
    void attack() override;
    std::string getType() const override { return "Mago"; }

    void performSpecialAttack() override;

};

#endif // MAGE_TOWER_H