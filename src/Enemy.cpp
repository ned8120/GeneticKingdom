#include "Enemy.h"
#include <cmath>

Enemy::Enemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex, int enemySize)
    : health(100), speed(30.0f), arrowResistance(0.0f), magicResistance(0.0f), artilleryResistance(0.0f),
      x(startX), y(startY), currentPathIndex(0), path(pathPoints), reachedEnd(false),
      texture(tex), size(enemySize), goldValue(10) {
    
    destRect = {static_cast<int>(x - size/2), static_cast<int>(y - size/2), size, size};
}

Enemy::~Enemy() {
    // No destruimos la textura aquí porque la gestiona EnemyManager
}

void Enemy::update(int deltaTime) {
    if (!isAlive() || reachedEnd || path.empty() || currentPathIndex >= path.size()) {
        return;
    }
    
    // Calcular el destino actual
    SDL_Point target = path[currentPathIndex];
    
    // Calcular dirección y distancia al objetivo
    float dx = target.x - x;
    float dy = target.y - y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    // Si hemos llegado al punto actual del camino
    if (distance < 5.0f) {
        currentPathIndex++;
        
        // Si hemos llegado al final del camino
        if (currentPathIndex >= path.size()) {
            reachedEnd = true;
            return;
        }
        
        // Actualizar el nuevo destino
        target = path[currentPathIndex];
        dx = target.x - x;
        dy = target.y - y;
        distance = std::sqrt(dx*dx + dy*dy);
    }
    
    // Normalizar la dirección
    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }
    
    // Calcular el movimiento basado en la velocidad y el tiempo transcurrido
    float moveDistance = speed * (deltaTime / 1000.0f);
    
    // Actualizar posición
    x += dx * moveDistance;
    y += dy * moveDistance;
    
    // Actualizar rectángulo de destino para renderizado
    destRect.x = static_cast<int>(x - size/2);
    destRect.y = static_cast<int>(y - size/2);
}

void Enemy::render(SDL_Renderer* renderer) const {
    if (!isAlive()) {
        return;
    }
    
    // Dibujar el enemigo usando su textura
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    } else {
        // Método de respaldo usando un rectángulo coloreado
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo por defecto
        SDL_RenderFillRect(renderer, &destRect);
    }
    
    // Dibujar barra de vida
    int healthBarWidth = size;
    int healthBarHeight = 5;
    float healthPercentage = static_cast<float>(health) / 100.0f;
    
    // Fondo de la barra de vida (gris)
    SDL_Rect healthBarBg = {
        destRect.x,
        destRect.y - 10,
        healthBarWidth,
        healthBarHeight
    };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &healthBarBg);
    
    // Barra de vida actual (verde)
    SDL_Rect healthBarFg = {
        destRect.x,
        destRect.y - 10,
        static_cast<int>(healthBarWidth * healthPercentage),
        healthBarHeight
    };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &healthBarFg);
}

int Enemy::takeDamage(int damage, std::string towerType) {
    float resistance = 0.0f;
    
    // Aplicar resistencia según el tipo de torre
    if (towerType == "Arquero") {
        resistance = arrowResistance;
    } else if (towerType == "Mago") {
        resistance = magicResistance;
    } else if (towerType == "Artillero") {
        resistance = artilleryResistance;
    }
    
    // Calcular daño real después de aplicar resistencia
    int actualDamage = static_cast<int>(damage * (1.0f - resistance));
    
    // Aplicar daño
    health -= actualDamage;
    if (health < 0) health = 0;
    
    return actualDamage;
}

float Enemy::distanceTo(int targetX, int targetY) const {
    float dx = targetX - x;
    float dy = targetY - y;
    return std::sqrt(dx*dx + dy*dy);
}

void Enemy::setPath(const std::vector<SDL_Point>& newPath) {
    path = newPath;
    currentPathIndex = 0;
    reachedEnd = false;
}