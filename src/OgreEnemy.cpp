#include "OgreEnemy.h"

OgreEnemy::OgreEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex)
    : Enemy(startX, startY, pathPoints, tex) {
    
    // Establecer atributos específicos del ogro
    health = BASE_HEALTH;
    speed = 20.0f;  // Lento
    
    // Resistencias (0.0 = sin resistencia, 1.0 = inmune)
    arrowResistance = 0.7f;      // Muy resistente a flechas
    magicResistance = 0.2f;      // Débil contra magia
    artilleryResistance = 0.3f;  // Débil contra artillería
    
    // Valor en oro
    goldValue = BASE_GOLD;
}