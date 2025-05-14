#include "HarpyEnemy.h"

HarpyEnemy::HarpyEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex)
    : Enemy(startX, startY, pathPoints, tex) {
    
    // Establecer atributos específicos de la harpía
    health = BASE_HEALTH;
    speed = 45.0f;  // Velocidad intermedia
    
    // Resistencias (0.0 = sin resistencia, 1.0 = inmune)
    arrowResistance = 0.3f;      // Resistencia moderada a flechas
    magicResistance = 0.3f;      // Resistencia moderada a magia
    artilleryResistance = 1.0f;  // Inmune a artillería (volando)
    
    // Valor en oro
    goldValue = BASE_GOLD;
}

int HarpyEnemy::takeDamage(int damage, std::string towerType) {
    // Las harpías solo pueden ser dañadas por arqueros y magos
    if (towerType == "Artillero") {
        return 0; // No recibe daño de artillería
    }
    
    return Enemy::takeDamage(damage, towerType);
}