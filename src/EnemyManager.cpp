#include "EnemyManager.h"
#include "OgreEnemy.h"
#include "DarkElfEnemy.h"
#include "HarpyEnemy.h"
#include "Tower.h"  
#include "MercenaryEnemy.h"
#include "Game.h"  // Añadir este include
#include <iostream>
#include <algorithm>  // Para std::sort


EnemyManager::EnemyManager(GameBoard* board, ResourceSystem* res, SDL_Renderer* renderer)
    : ogreTexture(nullptr), darkElfTexture(nullptr), harpyTexture(nullptr), mercenaryTexture(nullptr),
      resources(res), waveTimer(0), waveInterval(30000), enemiesPerWave(5), currentWave(0) {
    
    // Configurar punto de entrada desde el tablero
    entrancePoint = {board->getEntrancePoint().x * 50 + 25, board->getEntrancePoint().y * 50 + 25};
    
    // Generar caminos posibles
    generatePaths(board);
    
    // Inicializar generador de números aleatorios
    std::random_device rd;
    rng = std::mt19937(rd());
    enemyTypeDist = std::uniform_int_distribution<int>(0, 3);
    pathDist = std::uniform_int_distribution<int>(0, paths.size() - 1);
    
    // Cargar texturas
    loadTextures(renderer);
}

EnemyManager::~EnemyManager() {
    // Liberar texturas
    if (ogreTexture) SDL_DestroyTexture(ogreTexture);
    if (darkElfTexture) SDL_DestroyTexture(darkElfTexture);
    if (harpyTexture) SDL_DestroyTexture(harpyTexture);
    if (mercenaryTexture) SDL_DestroyTexture(mercenaryTexture);
}

bool EnemyManager::loadTextures(SDL_Renderer* renderer) {
    // Cargar las imágenes para cada tipo de enemigo
    SDL_Surface* surface = IMG_Load("images/ogro.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen ogro.png: " << IMG_GetError() << std::endl;
        return false;
    }
    ogreTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("images/elfo.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen elfo.png: " << IMG_GetError() << std::endl;
        return false;
    }
    darkElfTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("images/harpia.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen harpia.png: " << IMG_GetError() << std::endl;
        return false;
    }
    harpyTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("images/mercenario.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen mercenario.png: " << IMG_GetError() << std::endl;
        return false;
    }
    mercenaryTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return ogreTexture && darkElfTexture && harpyTexture && mercenaryTexture;
}

void EnemyManager::generatePaths(GameBoard* board) {
    // Limpiar caminos anteriores
    paths.clear();
    
    std::vector<SDL_Point> mainPath;
    
    // Punto de entrada en píxeles (centro de la celda)
    int gridSize = 50;  // Tamaño de cada celda
    SDL_Point entrance = board->getEntrancePoint();
    SDL_Point exit = board->getExitPoint();
    
    std::cout << "Generando caminos desde (" << entrance.x << "," << entrance.y 
              << ") hasta (" << exit.x << "," << exit.y << ")" << std::endl;
    
    // Versión mejorada: crear camino siguiendo exactamente las celdas de camino en el tablero
    // Primero obtenemos el camino de celdas usando A* o BFS
    std::vector<SDL_Point> gridPath;
    
    // Simplificado para este ejemplo: usamos una búsqueda en línea recta desde la entrada hasta la salida
    // (En una implementación completa, aquí usarías A*)
    
    // Punto inicial (entrada)
    gridPath.push_back(entrance);
    
    // Camino central - buscar celdas de camino consecutivas
    // Para cada columna desde la entrada hasta la salida
    for (int c = entrance.x + 1; c < exit.x; c++) {
        // Para cada fila, buscar una celda de camino
        for (int r = 0; r < board->getRows(); r++) {
            if (board->getCellType(r, c) == 1) {  // Si es un camino
                // Verificar si está conectado con la celda anterior en el camino
                if (!gridPath.empty()) {
                    SDL_Point lastCell = gridPath.back();
                    // Si es adyacente o está a una distancia de 1 celda
                    if ((abs(r - lastCell.y) + abs(c - lastCell.x)) <= 2) {
                        gridPath.push_back({c, r});
                        break;  // Solo agregar una celda por columna
                    }
                }
            }
        }
    }
    
    // Punto final (salida)
    gridPath.push_back(exit);
    
    // Ahora convertir las coordenadas de la cuadrícula a coordenadas de píxeles (centro de cada celda)
    for (const SDL_Point& gridCell : gridPath) {
        SDL_Point pixelPos = {
            gridCell.x * gridSize + gridSize/2,
            gridCell.y * gridSize + gridSize/2
        };
        mainPath.push_back(pixelPos);
        
        std::cout << "Punto de camino: coordenadas de grid (" << gridCell.x << "," << gridCell.y 
                  << ") -> píxeles (" << pixelPos.x << "," << pixelPos.y << ")" << std::endl;
    }
    
    // Agregar este camino a la lista
    paths.push_back(mainPath);
    
    // En la versión final, aquí generarías múltiples caminos con A*
    std::cout << "Camino generado con " << mainPath.size() << " puntos" << std::endl;
}

std::unique_ptr<Enemy> EnemyManager::createEnemy(EnemyType type, const std::vector<SDL_Point>& path) {
    // Crear un enemigo del tipo especificado
    switch (type) {
        case EnemyType::OGRE:
            return std::make_unique<OgreEnemy>(entrancePoint.x, entrancePoint.y, path, ogreTexture);
        case EnemyType::DARK_ELF:
            return std::make_unique<DarkElfEnemy>(entrancePoint.x, entrancePoint.y, path, darkElfTexture);
        case EnemyType::HARPY:
            return std::make_unique<HarpyEnemy>(entrancePoint.x, entrancePoint.y, path, harpyTexture);
        case EnemyType::MERCENARY:
            return std::make_unique<MercenaryEnemy>(entrancePoint.x, entrancePoint.y, path, mercenaryTexture);
        default:
            return std::make_unique<OgreEnemy>(entrancePoint.x, entrancePoint.y, path, ogreTexture);
    }
}

void EnemyManager::spawnWave() {
    currentWave++;
    std::cout << "¡Nueva oleada! Oleada #" << currentWave << std::endl;
    
    // Aumentar dificultad con cada oleada
    enemiesPerWave = 5 + currentWave;
    
    // Generar enemigos de tipos aleatorios
    for (int i = 0; i < enemiesPerWave; i++) {
        // Elegir un tipo de enemigo aleatoriamente
        EnemyType type = static_cast<EnemyType>(enemyTypeDist(rng));
        
        // Elegir un camino aleatorio
        int pathIndex = pathDist(rng);
        if (pathIndex >= paths.size()) pathIndex = 0;
        
        // Crear enemigo y agregarlo a la lista
        enemies.push_back(createEnemy(type, paths[pathIndex]));
        
        // Para evitar que aparezcan todos juntos, esperaríamos un tiempo entre cada enemigo
        // En una implementación completa, esto se manejaría con un temporizador
    }
}

void EnemyManager::update(int deltaTime) {
    // Actualizar temporizador de oleadas
    waveTimer += deltaTime;
    if (waveTimer >= waveInterval) {
        spawnWave();
        waveTimer = 0;
    }
    
    // Actualizar todos los enemigos
    for (auto& enemy : enemies) {
        enemy->update(deltaTime);
    }
    
    // Eliminar enemigos muertos y procesar oro
    auto it = enemies.begin();
    while (it != enemies.end()) {
        if (!(*it)->isAlive()) {
            // Añadir oro al matar un enemigo
            resources->addGold((*it)->getGoldValue());
            std::cout << "Enemigo eliminado. +" << (*it)->getGoldValue() << " oro." << std::endl;
            it = enemies.erase(it);
        } else if ((*it)->hasReachedEnd()) {
            // El enemigo llegó al final (daño al jugador)
            std::cout << "¡Un enemigo ha cruzado el puente!" << std::endl;
            it = enemies.erase(it);
            // Aquí se podría implementar un sistema de vidas
        } else {
            ++it;
        }
    }
}

void EnemyManager::render(SDL_Renderer* renderer) const {
    // Renderizar todos los enemigos
    for (const auto& enemy : enemies) {
        enemy->render(renderer);
    }
}

void EnemyManager::processTowerAttacks(const std::vector<std::unique_ptr<Tower>>& towers, Game* game) {
    // Para depuración
    bool foundTargets = false;
    
    // DEPURACIÓN EXTREMA: Mostrar posiciones exactas
    std::cout << "===== DEPURACIÓN EXTREMA =====" << std::endl;
    std::cout << "Número de torres: " << towers.size() << std::endl;
    std::cout << "Número de enemigos: " << enemies.size() << std::endl;
    
    // Iterar por todas las torres
    for (const auto& tower : towers) {
        // Obtener posición y rango de la torre
        int towerCenterX = (tower->getCol() * 50) + 25;
        int towerCenterY = (tower->getRow() * 50) + 25;
        int towerRange = tower->getRange();
        
        std::cout << "Torre " << tower->getType() << " en (" << towerCenterX << "," << towerCenterY 
                  << "), puede atacar: " << (tower->canAttack() ? "SÍ" : "NO") << std::endl;
        
        // Verificar si la torre puede atacar según su temporizador interno
        if (tower->canAttack()) {
            // Buscar enemigos para atacar
            if (!enemies.empty()) {
                for (auto& enemy : enemies) {
                    if (enemy->isAlive()) {
                        SDL_Point enemyPos = enemy->getPosition();
                        
                        // Calcular distancia real usando la fórmula euclidiana
                        float dx = enemyPos.x - towerCenterX;
                        float dy = enemyPos.y - towerCenterY;
                        float distance = std::sqrt(dx*dx + dy*dy);
                        
                        // Información detallada de la posición del enemigo
                        std::cout << "  Enemigo " << enemy->getType() << " en (" << enemyPos.x << "," 
                                << enemyPos.y << "), Distancia total: " << distance 
                                << ", Rango de la torre: " << towerRange << std::endl;
                        
                        // Usar el rango completo de la torre
                        if (distance <= towerRange) {
                            foundTargets = true;
                            
                            // Efectos visuales del ataque
                            tower->visualAttack();
                            
                            // IMPORTANTE: Aplicar daño al enemigo
                            int damage = tower->getDamage();
                            std::cout << ">>> APLICANDO DAÑO de " << damage << " al " << enemy->getType() << " <<<" << std::endl;
                            int actualDamage = enemy->takeDamage(damage, tower->getType());
                            
                            // Mostrar información de daño
                            std::cout << "¡¡ATAQUE EXITOSO!! " << tower->getType() << " ataca a " << enemy->getType() 
                                    << " causando " << actualDamage << " de daño!" << std::endl;
                            
                            // NUEVO: Añadir mensaje en pantalla
                            if (game) {
                                std::string message = tower->getType() + " → " + enemy->getType() + 
                                                     " (-" + std::to_string(actualDamage) + " HP) | Vida: " + 
                                                     std::to_string(enemy->getHealth()) + " HP";
                                
                                // Color según el tipo de torre
                                SDL_Color color = {255, 255, 255, 255}; // Blanco por defecto
                                if (tower->getType() == "Arquero") {
                                    color = {0, 150, 0, 255}; // Verde para arqueros
                                } else if (tower->getType() == "Mago") {
                                    color = {0, 0, 200, 255}; // Azul para magos
                                } else if (tower->getType() == "Artillero") {
                                    color = {200, 0, 0, 255}; // Rojo para artilleros
                                }
                                
                                game->addAttackMessage(message, color);
                            }
                            
                            // Reiniciar el temporizador después del ataque
                            tower->resetTimer();
                            
                            // Si el enemigo murió, dar oro y mostrar mensaje
                            if (!enemy->isAlive()) {
                                int goldGained = enemy->getGoldValue();
                                resources->addGold(goldGained);
                                std::cout << "¡¡ENEMIGO ELIMINADO!! +" << goldGained << " de oro." << std::endl;
                                
                                // NUEVO: Mensaje de enemigo eliminado
                                if (game) {
                                    std::string message = "¡" + enemy->getType() + " eliminado! +" + 
                                                         std::to_string(goldGained) + " oro";
                                    SDL_Color goldColor = {255, 215, 0, 255}; // Dorado
                                    game->addAttackMessage(message, goldColor);
                                }
                            }
                            
                            // Romper el bucle para que una torre sólo ataque a un enemigo por ciclo
                            break;
                        }
                    }
                }
            }
        }
    }
    
    if (!foundTargets && !enemies.empty()) {
        std::cout << "¡Ningún enemigo en rango de ninguna torre! (Tenemos " << enemies.size() << " enemigos activos)" << std::endl;
    }
}


int EnemyManager::getEnemiesKilled() const {
    // Implementación para estadísticas
    // Por ahora devolvemos 0, pero en el futuro mantendríamos un contador
    return 0;
}

void EnemyManager::spawnTestEnemies() {
    // Para pruebas: generar un enemigo de cada tipo
    if (!paths.empty()) {
        enemies.push_back(std::make_unique<OgreEnemy>(entrancePoint.x, entrancePoint.y, paths[0], ogreTexture));
        enemies.push_back(std::make_unique<DarkElfEnemy>(entrancePoint.x - 50, entrancePoint.y, paths[0], darkElfTexture));
        enemies.push_back(std::make_unique<HarpyEnemy>(entrancePoint.x - 100, entrancePoint.y, paths[0], harpyTexture));
        enemies.push_back(std::make_unique<MercenaryEnemy>(entrancePoint.x - 150, entrancePoint.y, paths[0], mercenaryTexture));
        
        std::cout << "Enemigos de prueba generados" << std::endl;
    }
}