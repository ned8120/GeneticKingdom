#include "EnemyManager.h"
#include "OgreEnemy.h"
#include "DarkElfEnemy.h"
#include "HarpyEnemy.h"
#include "Tower.h"  
#include "MercenaryEnemy.h"
#include "Game.h"  // Añadir este include
#include <iostream>
#include <algorithm>  // Para std::sort
#include <map>



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
    
    // Inicializar el algoritmo genético y el contador de ID
    geneticAlgorithm = GeneticAlgorithm(20, 0.1f, 0.7f, 2);
    nextEnemyId = 1;
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

    generatePathsWithAStar(board);

    // Limpiar caminos anteriores
    // paths.clear();
    
    // std::vector<SDL_Point> mainPath;
    
    // // Punto de entrada en píxeles (centro de la celda)
    // int gridSize = 50;  // Tamaño de cada celda
    // SDL_Point entrance = board->getEntrancePoint();
    // SDL_Point exit = board->getExitPoint();
    
    // std::cout << "Generando caminos desde (" << entrance.x << "," << entrance.y 
    //           << ") hasta (" << exit.x << "," << exit.y << ")" << std::endl;
    
    // // Versión mejorada: crear camino siguiendo exactamente las celdas de camino en el tablero
    // // Primero obtenemos el camino de celdas usando A* o BFS
    // std::vector<SDL_Point> gridPath;
    
    // // Simplificado para este ejemplo: usamos una búsqueda en línea recta desde la entrada hasta la salida
    // // (En una implementación completa, aquí usarías A*)
    
    // // Punto inicial (entrada)
    // gridPath.push_back(entrance);
    
    // // Camino central - buscar celdas de camino consecutivas
    // // Para cada columna desde la entrada hasta la salida
    // for (int c = entrance.x + 1; c < exit.x; c++) {
    //     // Para cada fila, buscar una celda de camino
    //     for (int r = 0; r < board->getRows(); r++) {
    //         if (board->getCellType(r, c) == 1) {  // Si es un camino
    //             // Verificar si está conectado con la celda anterior en el camino
    //             if (!gridPath.empty()) {
    //                 SDL_Point lastCell = gridPath.back();
    //                 // Si es adyacente o está a una distancia de 1 celda
    //                 if ((abs(r - lastCell.y) + abs(c - lastCell.x)) <= 2) {
    //                     gridPath.push_back({c, r});
    //                     break;  // Solo agregar una celda por columna
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // // Punto final (salida)
    // gridPath.push_back(exit);
    
    // // Ahora convertir las coordenadas de la cuadrícula a coordenadas de píxeles (centro de cada celda)
    // for (const SDL_Point& gridCell : gridPath) {
    //     SDL_Point pixelPos = {
    //         gridCell.x * gridSize + gridSize/2,
    //         gridCell.y * gridSize + gridSize/2
    //     };
    //     mainPath.push_back(pixelPos);
        
    //     std::cout << "Punto de camino: coordenadas de grid (" << gridCell.x << "," << gridCell.y 
    //               << ") -> píxeles (" << pixelPos.x << "," << pixelPos.y << ")" << std::endl;
    // }
    
    // // Agregar este camino a la lista
    // paths.push_back(mainPath);
    
    // // En la versión final, aquí generarías múltiples caminos con A*
    // std::cout << "Camino generado con " << mainPath.size() << " puntos" << std::endl;

}



void EnemyManager::generatePathsWithAStar(GameBoard* board) {
    // Limpiar caminos anteriores
    paths.clear();
    
    // Obtener puntos de entrada y salida
    SDL_Point entrance = board->getEntrancePoint();
    SDL_Point exit = board->getExitPoint();
    
    std::cout << "Generando caminos con A* desde (" << entrance.x << "," << entrance.y 
              << ") hasta (" << exit.x << "," << exit.y << ")" << std::endl;
    
    // Función lambda para verificar si una celda es caminable
    auto isWalkable = [board](int x, int y) {
        return board->isCellWalkable(x, y);
    };
    
    // Encontrar el camino principal usando A*
    std::vector<SDL_Point> mainGridPath = AStar::findPath(
        isWalkable, entrance, exit, board->getCols(), board->getRows());
    
    if (mainGridPath.empty()) {
        std::cerr << "¡Error! A* no pudo encontrar un camino." << std::endl;
        return;
    }
    
    std::cout << "A* encontró un camino con " << mainGridPath.size() << " puntos." << std::endl;
    
    // Convertir las coordenadas de grid a coordenadas de píxeles (centro de cada celda)
    int gridSize = 50;
    std::vector<SDL_Point> mainPixelPath;
    
    for (const SDL_Point& gridCell : mainGridPath) {
        SDL_Point pixelPos = {
            gridCell.x * gridSize + gridSize/2,
            gridCell.y * gridSize + gridSize/2
        };
        mainPixelPath.push_back(pixelPos);
        
        std::cout << "Punto de camino: coordenadas de grid (" << gridCell.x << "," << gridCell.y 
                  << ") -> píxeles (" << pixelPos.x << "," << pixelPos.y << ")" << std::endl;
    }
    
    // Agregar este camino a la lista
    paths.push_back(mainPixelPath);
    
    // Generar caminos alternativos (desviaciones)
    // Intentamos generar al menos 2 caminos alternativos
    generateAlternativePaths(board, entrance, exit);
    
    std::cout << "Total de caminos generados: " << paths.size() << std::endl;
}

void EnemyManager::generateAlternativePaths(GameBoard* board, SDL_Point entrance, SDL_Point exit) {
    int gridSize = 50;
    
    // Función para comprobar si una celda es caminable
    auto isWalkable = [board](int x, int y) {
        return board->isCellWalkable(x, y);
    };
    
    // Intentar generar caminos alternativos añadiendo "obstáculos temporales"
    // en diferentes puntos del camino principal
    std::vector<SDL_Point> mainGridPath = AStar::findPath(
        isWalkable, entrance, exit, board->getCols(), board->getRows());
    
    if (mainGridPath.size() < 4) {
        // Camino demasiado corto para generar alternativas
        return;
    }
    
    // Intentamos bloquear temporalmente algunos puntos del camino principal
    // para forzar caminos alternativos
    for (size_t i = 1; i < mainGridPath.size() - 1; i += 2) {
        SDL_Point blockedPoint = mainGridPath[i];
        
        // Crear una copia de la función isWalkable que evite el punto bloqueado
        auto alternativeIsWalkable = [board, blockedPoint](int x, int y) {
            if (x == blockedPoint.x && y == blockedPoint.y) {
                return false;  // Este punto está "bloqueado" temporalmente
            }
            return board->isCellWalkable(x, y);
        };
        
        // Intentar encontrar un camino alternativo
        std::vector<SDL_Point> altGridPath = AStar::findPath(
            alternativeIsWalkable, entrance, exit, board->getCols(), board->getRows());
        
        // Si encontramos un camino alternativo y es diferente del principal
        if (!altGridPath.empty() && altGridPath != mainGridPath) {
            // Convertir a coordenadas de píxeles
            std::vector<SDL_Point> altPixelPath;
            for (const SDL_Point& gridCell : altGridPath) {
                SDL_Point pixelPos = {
                    gridCell.x * gridSize + gridSize/2,
                    gridCell.y * gridSize + gridSize/2
                };
                altPixelPath.push_back(pixelPos);
            }
            
            // Añadir a la lista de caminos si es lo suficientemente diferente
            if (isPathSufficientlyDifferent(altPixelPath)) {
                paths.push_back(altPixelPath);
                std::cout << "Camino alternativo generado con " << altPixelPath.size() << " puntos." << std::endl;
            }
            
            // Si ya tenemos suficientes caminos, terminamos
            if (paths.size() >= 3) {
                break;
            }
        }
    }
}

bool EnemyManager::isPathSufficientlyDifferent(const std::vector<SDL_Point>& newPath) {
    // Verifica si un camino es significativamente diferente de los existentes
    if (paths.empty()) {
        return true;
    }
    
    for (const auto& existingPath : paths) {
        // Contar cuántos puntos son diferentes
        int differentPoints = 0;
        size_t minLength = std::min(existingPath.size(), newPath.size());
        
        for (size_t i = 0; i < minLength; i++) {
            int dx = existingPath[i].x - newPath[i].x;
            int dy = existingPath[i].y - newPath[i].y;
            if (dx*dx + dy*dy > 100) {  // Si están a más de 10 píxeles de distancia
                differentPoints++;
            }
        }
        
        // Si menos del 30% de los puntos son diferentes, consideramos que es muy similar
        if (differentPoints < minLength * 0.3) {
            return false;
        }
    }
    
    return true;
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
    
    // Actualizar la dificultad con cada oleada
    enemiesPerWave = 5 + currentWave;
    
    // Si no es la primera oleada, evolucionar la población
    if (currentWave > 1) {
        // CORRECCIÓN: Crear un mapa para transferir los datos de fitness a la población real
        std::map<int, float> fitnessMap;  // Mapa de tipo de enemigo -> fitness
        std::map<int, int> countMap;      // Contador para promediar
        
        // Procesar los datos de rendimiento
        for (auto& data : enemyPerformanceData) {
            // Calcular fitness
            float fitness = data.progressMade * 10.0f + (data.damageDealt / 100.0f) + (data.timeAlive / 1000.0f);
            
            std::cout << "DEBUG - UpdateFitness con valores: progreso=" << data.progressMade 
                     << ", daño=" << data.damageDealt 
                     << ", tiempo=" << data.timeAlive << std::endl;
            std::cout << "Fitness actualizado: " << fitness << std::endl;
            
            // Acumular fitness por tipo de enemigo para después transferirlo a la población
            int enemyType = data.genome.enemyType;
            fitnessMap[enemyType] += fitness;
            countMap[enemyType]++;
        }
        
        // CORRECCIÓN: Aplicar el fitness a la población real
        std::vector<Genome>& population = geneticAlgorithm.getPopulationRef();
        for (auto& genome : population) {
            int type = genome.enemyType;
            if (countMap[type] > 0) {
                // Asignar el fitness promedio para este tipo de enemigo
                genome.fitness = fitnessMap[type] / countMap[type];
            }
        }
        
        // Evolucionar la población
        geneticAlgorithm.evolve();
        
        // Limpiar datos de rendimiento para la siguiente oleada
        enemyPerformanceData.clear();
    }
    
    // Generar enemigos basados en genomas
    const std::vector<Genome>& population = geneticAlgorithm.getPopulation();
    int enemiesToSpawn = std::min(enemiesPerWave, static_cast<int>(population.size()));
    
    std::cout << "Generando " << enemiesToSpawn << " enemigos para la oleada #" << currentWave << std::endl;
    
    for (int i = 0; i < enemiesToSpawn; i++) {
        // Elegir un camino aleatorio
        int pathIndex = pathDist(rng);
        if (pathIndex >= paths.size()) pathIndex = 0;
        
        // Crear y registrar el enemigo
        auto enemy = createEnemyFromGenome(population[i], paths[pathIndex]);
        
        // Registrar el genoma para este enemigo
        EnemyPerformance perf;
        perf.id = nextEnemyId;
        perf.genome = population[i];
        perf.progressMade = 0.0f;
        perf.damageDealt = 0.0f;
        perf.timeAlive = 0.0f;
        enemyPerformanceData.push_back(perf);
        
        // Asignar ID al enemigo y añadirlo a la lista
        enemy->setId(nextEnemyId++);
        enemies.push_back(std::move(enemy));
    }
}



std::unique_ptr<Enemy> EnemyManager::createEnemyFromGenome(const Genome& genome, const std::vector<SDL_Point>& path) {
    std::unique_ptr<Enemy> enemy;
    
    // Crear el tipo base de enemigo según el genoma
    switch (genome.enemyType) {
        case 0: // Ogro
            enemy = std::make_unique<OgreEnemy>(entrancePoint.x, entrancePoint.y, path, ogreTexture);
            break;
        case 1: // Elfo Oscuro
            enemy = std::make_unique<DarkElfEnemy>(entrancePoint.x, entrancePoint.y, path, darkElfTexture);
            break;
        case 2: // Harpía
            enemy = std::make_unique<HarpyEnemy>(entrancePoint.x, entrancePoint.y, path, harpyTexture);
            break;
        case 3: // Mercenario
            enemy = std::make_unique<MercenaryEnemy>(entrancePoint.x, entrancePoint.y, path, mercenaryTexture);
            break;
        default:
            enemy = std::make_unique<OgreEnemy>(entrancePoint.x, entrancePoint.y, path, ogreTexture);
    }
    
    // Aplicar atributos del genoma
    enemy->setHealth(static_cast<int>(genome.health));
    enemy->setSpeed(genome.speed);
    enemy->setArrowResistance(genome.arrowResistance);
    enemy->setMagicResistance(genome.magicResistance);
    enemy->setArtilleryResistance(genome.artilleryResistance);
    
    std::cout << "Enemigo creado de genoma: Tipo=" << genome.enemyType 
              << ", Vida=" << genome.health 
              << ", Velocidad=" << genome.speed 
              << ", Resistencias=" << genome.arrowResistance << "/" 
              << genome.magicResistance << "/" 
              << genome.artilleryResistance << std::endl;
    
    return enemy;
}


void EnemyManager::registerEnemyDeath(int enemyId, float progressMade, float damageDealt, float timeAlive) {
    // Encontrar el enemigo en los datos de rendimiento
    for (auto& data : enemyPerformanceData) {
        if (data.id == enemyId) {
            // Actualizar datos de rendimiento
            data.progressMade = progressMade;
            data.damageDealt = damageDealt;
            data.timeAlive = timeAlive;
            
            std::cout << "Rendimiento registrado para enemigo #" << enemyId 
                      << ": Progreso=" << progressMade 
                      << ", Daño=" << damageDealt 
                      << ", Tiempo=" << timeAlive << std::endl;
            return;
        }
    }
    
    std::cerr << "Error: Enemigo #" << enemyId << " no encontrado en los datos de rendimiento" << std::endl;
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
        
        // Actualizar tiempo de vida para el cálculo de fitness
        for (auto& data : enemyPerformanceData) {
            if (data.id == enemy->getId()) {
                data.timeAlive += deltaTime;
                // Actualizar el progreso en el camino
                data.progressMade = enemy->getPathProgress();
                break;
            }
        }
    }
    
    // Eliminar enemigos muertos y procesar oro
    auto it = enemies.begin();
    while (it != enemies.end()) {
        if (!(*it)->isAlive()) {
            // Registrar rendimiento final
            registerEnemyDeath((*it)->getId(), 
                              (*it)->getPathProgress(), 
                              (*it)->getDamageDealt(), 
                              0.0f); // El tiempo ya se ha acumulado
            
            // Añadir oro al matar un enemigo
            resources->addGold((*it)->getGoldValue());
            std::cout << "Enemigo #" << (*it)->getId() << " eliminado. +" << (*it)->getGoldValue() << " oro." << std::endl;
            it = enemies.erase(it);
        } else if ((*it)->hasReachedEnd()) {
            // El enemigo llegó al final - éxito máximo para su fitness
            registerEnemyDeath((*it)->getId(), 
                              1.0f, // Progreso máximo
                              (*it)->getDamageDealt(), 
                              0.0f); // El tiempo ya se ha acumulado
            
            std::cout << "¡Enemigo #" << (*it)->getId() << " ha cruzado el puente!" << std::endl;
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
            // Verificar si se activará un ataque especial
            bool specialAttack = tower->isSpecialAttackReady();
            
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
                            
                            // Determinar tipo de ataque
                            if (specialAttack) {
                                // Efectos visuales del ataque especial
                                tower->visualSpecialAttack();
                                
                                // IMPORTANTE: Aplicar daño al enemigo (modificar según tipo de torre)
                                int baseDamage = tower->getDamage();
                                int multiplier = 1;
                                
                                // Aplicar multiplicador según tipo de torre
                                if (tower->getType() == "Arquero") {
                                    multiplier = 3; // Lluvia de flechas
                                } else if (tower->getType() == "Mago") {
                                    multiplier = 2; // Explosión arcana
                                } else if (tower->getType() == "Artillero") {
                                    multiplier = 2; // Proyectil aturdidor
                                }
                                
                                int damage = baseDamage * multiplier;
                                std::cout << ">>> APLICANDO DAÑO ESPECIAL de " << damage << " al " << enemy->getType() << " <<<" << std::endl;
                                int actualDamage = enemy->takeDamage(damage, tower->getType());
                                
                                // Mostrar información de daño
                                std::cout << "¡¡ATAQUE ESPECIAL EXITOSO!! " << tower->getType() << " ataca a " << enemy->getType() 
                                        << " causando " << actualDamage << " de daño!" << std::endl;
                                
                                // Añadir mensaje en pantalla
                                if (game) {
                                    std::string message = "¡ESPECIAL! " + tower->getType() + " → " + enemy->getType() + 
                                                        " (-" + std::to_string(actualDamage) + " HP) | Vida: " + 
                                                        std::to_string(enemy->getHealth()) + " HP";
                                    
                                    // Color brillante para ataques especiales
                                    SDL_Color color = {255, 255, 0, 255}; // Amarillo brillante
                                    game->addAttackMessage(message, color);
                                }
                            } else {
                                // Efectos visuales del ataque normal
                                tower->visualAttack();
                                
                                // IMPORTANTE: Aplicar daño normal al enemigo
                                int damage = tower->getDamage();
                                std::cout << ">>> APLICANDO DAÑO de " << damage << " al " << enemy->getType() << " <<<" << std::endl;
                                int actualDamage = enemy->takeDamage(damage, tower->getType());
                                
                                // Mostrar información de daño
                                std::cout << "¡¡ATAQUE EXITOSO!! " << tower->getType() << " ataca a " << enemy->getType() 
                                        << " causando " << actualDamage << " de daño!" << std::endl;
                                
                                // Añadir mensaje en pantalla
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
                            }
                            
                            // Reiniciar el temporizador después del ataque
                            tower->resetTimer();
                            
                            // Si el enemigo murió, dar oro y mostrar mensaje
                            if (!enemy->isAlive()) {
                                int goldGained = enemy->getGoldValue();
                                resources->addGold(goldGained);
                                std::cout << "¡¡ENEMIGO ELIMINADO!! +" << goldGained << " de oro." << std::endl;
                                
                                // Mensaje de enemigo eliminado
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