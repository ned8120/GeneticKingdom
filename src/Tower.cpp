#include "Tower.h"
#include <random>
#include <iostream>

Tower::Tower(int r, int c, int initialCost, SDL_Texture* tex) 
    : level(1), row(r), col(c), cost(initialCost), upgradeCost(initialCost/2),
      attackTimer(0), texture(tex), specialAttackProbability(0.1f), 
      specialAttackTimer(0), specialAttackReady(false) {
    // Valores base serán asignados por las subclases
}

Tower::~Tower() {
    // No destruimos la textura aquí porque la gestiona TowerManager
}

void Tower::render(SDL_Renderer* renderer, int gridSize) const {
    // Dibujar la torre usando la textura si está disponible
    if (texture) {
        SDL_Rect destRect = {col * gridSize, row * gridSize, gridSize, gridSize};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    } else {
        // Método de respaldo usando color (como antes)
        SDL_Rect towerRect = {col * gridSize, row * gridSize, gridSize, gridSize};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &towerRect);
    }
    
    // Dibuja un indicador del nivel (líneas en la parte superior)
    for (int i = 0; i < level; i++) {
        SDL_Rect levelIndicator = {
            col * gridSize + (i * gridSize/3), 
            row * gridSize, 
            gridSize/4, 
            gridSize/10
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco
        SDL_RenderFillRect(renderer, &levelIndicator);
    }
    
    // Dibuja borde
    SDL_Rect towerRect = {col * gridSize, row * gridSize, gridSize, gridSize};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    SDL_RenderDrawRect(renderer, &towerRect);
    
    // Dibujar radio de alcance (círculo transparente)
    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 30); // Blanco semi-transparente
        
        // Dibujar un círculo usando el algoritmo de Midpoint
        int centerX = col * gridSize + gridSize / 2;
        int centerY = row * gridSize + gridSize / 2;
        
        // Dibujar aproximación de círculo
        for (int w = -range; w <= range; w++) {
            for (int h = -range; h <= range; h++) {
                if (w*w + h*h <= range*range) {
                    SDL_RenderDrawPoint(renderer, centerX + w, centerY + h);
                }
            }
        }
        
        // Restaurar modo de mezcla
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

void Tower::update(int deltaTime) {
    // Incrementar temporizadores
    attackTimer += deltaTime;
    specialAttackTimer += deltaTime;
    
    // Verificar si es hora de un ataque especial
    if (specialAttackTimer >= specialCooldown) {
        trySpecialAttack();
    }
}

bool Tower::upgrade() {
    // Solo puede mejorarse hasta nivel 3
    if (level >= 3) {
        return false;
    }
    
    // Actualiza nivel y aumenta estadísticas
    level++;
    damage += damage * 0.3; // 30% más de daño
    range += 10; // Aumenta ligeramente el alcance
    attackSpeed -= attackSpeed * 0.1; // 10% más rápido
    
    // Mejorar ataque especial
    specialAttackProbability += 0.1f; // Mayor probabilidad
    specialCooldown -= specialCooldown * 0.1f; // Menor tiempo de recarga
    
    return true;
}

int Tower::getUpgradeCost() const {
    // Cada nivel es más caro de mejorar
    return upgradeCost * level;
}

std::string Tower::getTypeString() const {
    return getType() + " Nivel " + std::to_string(level);
}


// Añadir estas funciones al final de Tower.cpp
bool Tower::trySpecialAttack() {
    // Actualizar temporizador de ataque especial
    if (specialAttackTimer >= specialCooldown) {
        // Verificar probabilidad
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        if (dist(gen) < specialAttackProbability) {
            // Activar ataque especial
            specialAttackReady = true;
            return true;
        }
    }
    return false;
}

void Tower::visualSpecialAttack() {
    // Llamar a la implementación específica
    performSpecialAttack();
    
    // Reiniciar temporizadores
    specialAttackTimer = 0;
    specialAttackReady = false;
}