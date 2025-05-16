#include "MageTower.h"
#include <iostream>

MageTower::MageTower(int r, int c, SDL_Texture* tex) : Tower(r, c, BASE_COST, tex) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    specialAttackProbability = 0.12f;  // 12% de probabilidad base
    
    // Color azul para los magos (como respaldo)
    color = {0, 0, 200, 255};
}

void MageTower::attack() {
    // Verificar si hay un ataque especial listo
    if (specialAttackReady) {
        performSpecialAttack();
        specialAttackReady = false;
        return;
    }
    
    // Lógica de ataque normal
    std::cout << "¡Mago lanza hechizo! Daño: " << damage << std::endl;
}

void MageTower::performSpecialAttack() {
    // Lógica de ataque especial: explosión mágica (daño en área)
    std::cout << "¡¡ATAQUE ESPECIAL!! Mago lanza explosión arcana! "
              << "Daño: " << (damage * 1.5) << " en un radio de " 
              << AREA_DAMAGE_RADIUS << " píxeles" << std::endl;
    
    // En una implementación completa, aquí buscaríamos todos los enemigos
    // dentro del radio y les aplicaríamos daño
}
