#include "ArcherTower.h"
#include <iostream>

ArcherTower::ArcherTower(int r, int c, SDL_Texture* tex) : Tower(r, c, BASE_COST, tex) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    
    // Color verde oscuro para los arqueros (como respaldo)
    color = {0, 100, 0, 255};
}

void ArcherTower::attack() {
    // Lógica de ataque (por ahora solo imprime)
    std::cout << "¡Arquero ataca! Daño: " << damage << std::endl;
    
    // En el futuro, aquí buscaríamos enemigos en rango y les aplicaríamos daño
}