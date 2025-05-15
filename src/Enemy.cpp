#include "Enemy.h"
#include "GameBoard.h"  // Include the full GameBoard definition
#include "AStar.h"      // Include for A* pathfinding
#include <cmath>
#include <iostream>

Enemy::Enemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex, int enemySize)
    : health(100), speed(30.0f), arrowResistance(0.0f), magicResistance(0.0f), artilleryResistance(0.0f),
      x(startX), y(startY), currentPathIndex(0), path(pathPoints), reachedEnd(false),
      texture(tex), size(enemySize), goldValue(10), gameBoard(nullptr), id(0), damageDealt(0.0f) {
    
    destRect = {static_cast<int>(x - size/2), static_cast<int>(y - size/2), size, size};
}

Enemy::~Enemy() {
    // No destruimos la textura aquí porque la gestiona EnemyManager
}

void Enemy::update(int deltaTime) {
    if (!isAlive() || reachedEnd || path.empty() || currentPathIndex >= static_cast<int>(path.size())) {
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
        if (currentPathIndex >= static_cast<int>(path.size())) {
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
        // Método alternativo usando un rectángulo de color
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo por defecto
        SDL_RenderFillRect(renderer, &destRect);
    }
    
    // MEJORADO: Asegúrate de que la barra de vida sea muy visible
    int healthBarWidth = size;
    int healthBarHeight = 10; // Bien visible
    float healthPercentage = static_cast<float>(health) / 100.0f;
    
    // Fondo de la barra de vida (gris oscuro)
    SDL_Rect healthBarBg = {
        destRect.x,
        destRect.y - 15, // Un poco más arriba para que se vea bien
        healthBarWidth,
        healthBarHeight
    };
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(renderer, &healthBarBg);
    
    // Barra de vida actual (verde a rojo basado en la vida)
    SDL_Rect healthBarFg = {
        destRect.x,
        destRect.y - 15,
        static_cast<int>(healthBarWidth * healthPercentage),
        healthBarHeight
    };
    
    // Color dinámico: verde (100% vida) -> amarillo (50% vida) -> rojo (0% vida)
    Uint8 r = static_cast<Uint8>((1.0f - healthPercentage) * 255);
    Uint8 g = static_cast<Uint8>(healthPercentage * 255);
    SDL_SetRenderDrawColor(renderer, r, g, 0, 255);
    SDL_RenderFillRect(renderer, &healthBarFg);
    
    // Borde negro para mejor visibilidad
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &healthBarBg);
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
    
    // Guardar valor anterior de vida para depuración
    int oldHealth = health;
    
    // Aplicar daño
    health -= actualDamage;
    if (health < 0) health = 0;
    
    // Si el enemigo llega al final, registra daño al jugador para su fitness
    if (hasReachedEnd()) {
        damageDealt += 10.0f; // Valor arbitrario de daño al jugador
    }
    
    // AÑADIR MÁS INFORMACIÓN DE DEPURACIÓN
    std::cout << "APLICANDO DAÑO: " << towerType << " → " << getType() 
              << " | Daño base: " << damage 
              << " | Resistencia: " << resistance 
              << " | Daño real: " << actualDamage 
              << " | Vida: " << oldHealth << " → " << health 
              << " (" << (oldHealth > 0 ? (health * 100 / oldHealth) : 0) << "% restante)" << std::endl;
    
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

void Enemy::recalculatePath(GameBoard* board) {
    if (!board || path.empty()) return;
    
    // Convertir posición actual a coordenadas de grid
    int gridSize = 50;
    int currentGridX = static_cast<int>(x) / gridSize;
    int currentGridY = static_cast<int>(y) / gridSize;
    
    // Obtener punto final (destino)
    SDL_Point finalGridPoint = {
        path.back().x / gridSize,
        path.back().y / gridSize
    };
    
    // Función lambda para verificar si una celda es caminable
    auto isWalkable = [board](int x, int y) {
        return board->isCellWalkable(x, y);
    };
    
    // Recalcular camino con A*
    std::vector<SDL_Point> newGridPath = AStar::findPath(
        isWalkable, {currentGridX, currentGridY}, finalGridPoint, 
        board->getCols(), board->getRows());
    
    if (newGridPath.empty()) {
        std::cout << "No se pudo recalcular camino para " << getType() << std::endl;
        return;
    }
    
    // Convertir a coordenadas de píxeles
    std::vector<SDL_Point> newPixelPath;
    for (const SDL_Point& gridPoint : newGridPath) {
        SDL_Point pixelPos = {
            gridPoint.x * gridSize + gridSize/2,
            gridPoint.y * gridSize + gridSize/2
        };
        newPixelPath.push_back(pixelPos);
    }
    
    // Establecer nuevo camino
    setPath(newPixelPath);
    std::cout << "Camino recalculado para " << getType() << " con " << newPixelPath.size() << " puntos" << std::endl;
}