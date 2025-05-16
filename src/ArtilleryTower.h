#ifndef ARTILLERY_TOWER_H
#define ARTILLERY_TOWER_H

#include "Tower.h"

class ArtilleryTower : public Tower {
private:
    static const int BASE_DAMAGE = 40;       // Alto daño
    static const int BASE_RANGE = 80;        // Bajo alcance
    static const int BASE_ATTACK_SPEED = 2000; // Ataque lento (ms)
    static const int BASE_SPECIAL_COOLDOWN = 15000; // 15 segundos
    static const int BASE_COST = 75;         // Alto costo
    static constexpr float STUN_DURATION = 2000.0f;


public:
    ArtilleryTower(int r, int c, SDL_Texture* tex = nullptr);
    
    // Implementación de métodos virtuales
    void attack() override;
    std::string getType() const override { return "Artillero"; }

    void performSpecialAttack() override;

};

#endif // ARTILLERY_TOWER_H