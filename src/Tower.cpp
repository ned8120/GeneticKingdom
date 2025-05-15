#include "Tower.h"

Tower::Tower(int r, int c, int initialCost, SDL_Texture* tex) 
    : level(1), row(r), col(c), cost(initialCost), upgradeCost(initialCost/2),
      attackTimer(0), texture(tex) {
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
    // Solo incrementar el temporizador, sin atacar automáticamente
    attackTimer += deltaTime;
    
    // Eliminar o comentar esta parte:
    // if (attackTimer >= attackSpeed) {
    //     attack();
    //     attackTimer = 0;
    // }
}

bool Tower::upgrade() {
    // Solo puede mejorarse hasta nivel 3
    if (level >= 3) {
        return false;
    }
    
    // Actualiza nivel y aumenta estadísticas (las subclases pueden sobrescribir esto)
    level++;
    damage += damage * 0.3; // 30% más de daño
    range += 10; // Aumenta ligeramente el alcance
    attackSpeed -= attackSpeed * 0.1; // 10% más rápido (menos tiempo entre ataques)
    
    return true;
}

int Tower::getUpgradeCost() const {
    // Cada nivel es más caro de mejorar
    return upgradeCost * level;
}

std::string Tower::getTypeString() const {
    return getType() + " Nivel " + std::to_string(level);
}