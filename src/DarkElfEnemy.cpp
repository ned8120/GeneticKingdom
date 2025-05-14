#include "DarkElfEnemy.h"

DarkElfEnemy::DarkElfEnemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex)
    : Enemy(startX, startY, pathPoints, tex) {
    
    // Establecer atributos específicos del elfo oscuro
    health = BASE_HEALTH;
    speed = 60.0f;  // Muy rápido
    
    // Resistencias (0.0 = sin resistencia, 1.0 = inmune)
    arrowResistance = 0.2f;      // Débil contra flechas
    magicResistance = 0.7f;      // Muy resistente a magia
    artilleryResistance = 0.3f;  // Débil contra artillería
    
    // Valor en oro
    goldValue = BASE_GOLD;
}