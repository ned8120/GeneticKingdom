#include "ArcherTower.h"
#include <iostream>

ArcherTower::ArcherTower(int r, int c, SDL_Texture* tex) : Tower(r, c, BASE_COST, tex) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    specialAttackProbability = 0.15f;  // 15% de probabilidad base
    
    // Color verde oscuro para los arqueros (como respaldo)
    color = {0, 100, 0, 255};
}

void ArcherTower::attack() {
    // Verificar si hay un ataque especial listo
    if (specialAttackReady) {
        performSpecialAttack();
        specialAttackReady = false;
        return;
    }
    
    // Lógica de ataque normal
    std::cout << "¡Arquero ataca! Daño: " << damage << std::endl;
}


void ArcherTower::performSpecialAttack() {
    // Lógica de ataque especial: lluvia de flechas (múltiples flechas simultáneas)
    std::cout << "¡¡ATAQUE ESPECIAL!! Arquero lanza " << SPECIAL_ARROWS 
              << " flechas simultáneas! Daño total: " << (damage * SPECIAL_ARROWS) << std::endl;
    
    // En una implementación completa, aquí buscaríamos múltiples enemigos en rango
    // y les aplicaríamos daño, o atacaríamos al mismo enemigo múltiples veces
}