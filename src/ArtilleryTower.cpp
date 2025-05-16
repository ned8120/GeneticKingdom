#include "ArtilleryTower.h"
#include <iostream>

ArtilleryTower::ArtilleryTower(int r, int c, SDL_Texture* tex) : Tower(r, c, BASE_COST, tex) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    specialAttackProbability = 0.10f;  // 10% de probabilidad base
    
    // Color rojo oscuro para los artilleros (como respaldo)
    color = {150, 0, 0, 255};
}

void ArtilleryTower::attack() {
    // Verificar si hay un ataque especial listo
    if (specialAttackReady) {
        performSpecialAttack();
        specialAttackReady = false;
        return;
    }
    
    // Lógica de ataque normal
    std::cout << "¡Artillero dispara cañón! Daño: " << damage << std::endl;
}

void ArtilleryTower::performSpecialAttack() {
    // Lógica de ataque especial: proyectil aturdidor
    std::cout << "¡¡ATAQUE ESPECIAL!! Artillero lanza proyectil aturdidor! "
              << "Daño: " << (damage * 1.2) << " y aturde por " 
              << (STUN_DURATION/1000) << " segundos" << std::endl;
    
    // En una implementación completa, aquí buscaríamos un enemigo,
    // le aplicaríamos daño y le añadiríamos un efecto de aturdimiento
}