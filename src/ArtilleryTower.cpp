#include "ArtilleryTower.h"
#include <iostream>

ArtilleryTower::ArtilleryTower(int r, int c) : Tower(r, c, BASE_COST) {
    // Inicializa atributos específicos
    damage = BASE_DAMAGE;
    range = BASE_RANGE;
    attackSpeed = BASE_ATTACK_SPEED;
    specialCooldown = BASE_SPECIAL_COOLDOWN;
    
    // Color rojo oscuro para los artilleros
    color = {150, 0, 0, 255};
}

void ArtilleryTower::attack() {
    // Lógica de ataque (por ahora solo imprime)
    std::cout << "¡Artillero dispara cañón! Daño: " << damage << std::endl;
    
    // En el futuro, aquí buscaríamos enemigos en rango y les aplicaríamos daño
}