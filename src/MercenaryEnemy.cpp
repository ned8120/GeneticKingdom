#include "MercenaryEnemy.h"

MercenaryEnemy::MercenaryEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex)
    : Enemy(startX, startY, pathPoints, tex) {
    
    // Establecer atributos específicos del mercenario
    health = BASE_HEALTH;
    speed = 35.0f;  // Velocidad normal
    
    // Resistencias (0.0 = sin resistencia, 1.0 = inmune)
    arrowResistance = 0.6f;      // Resistente a flechas
    magicResistance = 0.2f;      // Débil contra magia
    artilleryResistance = 0.6f;  // Resistente a artillería
    
    // Valor en oro
    goldValue = BASE_GOLD;
}