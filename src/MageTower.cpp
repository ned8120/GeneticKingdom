#include "MageTower.h"
#include <iostream>

MageTower::MageTower(int r, int c, SDL_Texture* tex) : Tower(r, c, BASE_COST, tex) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    
    // Color azul para los magos (como respaldo)
    color = {0, 0, 200, 255};
}

void MageTower::attack() {
    // Lógica de ataque (por ahora solo imprime)
    std::cout << "¡Mago lanza hechizo! Daño: " << damage << std::endl;
    
    // En el futuro, aquí buscaríamos enemigos en rango y les aplicaríamos daño
}