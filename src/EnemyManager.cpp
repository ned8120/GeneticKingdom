#include "EnemyManager.h"
#include "OgreEnemy.h"
#include "DarkElfEnemy.h"
#include "HarpyEnemy.h"
#include "Tower.h"  
#include "MercenaryEnemy.h"
#include <iostream>

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
    // Por ahora, generamos un camino simple desde la entrada hasta la salida
    // Más adelante, esto usará A* para encontrar múltiples caminos posibles
    
    std::vector<SDL_Point> simplePath;
    
    // Punto de entrada en píxeles (centro de la celda)
    int gridSize = 50;  // Tamaño de cada celda
    SDL_Point entrance = board->getEntrancePoint();
    SDL_Point exit = board->getExitPoint();
    
    // Versión simplificada: camino directo a lo largo del camino central
    int midRow = board->getRows() / 2;
    
    // Agregar punto de entrada
    simplePath.push_back({entrance.x * gridSize + gridSize/2, entrance.y * gridSize + gridSize/2});
    
    // Agregar camino a lo largo de la fila central
    for (int c = 1; c < board->getCols() - 1; c++) {
        if (board->getCellType(midRow, c) == 1) {  // Si es un camino
            simplePath.push_back({c * gridSize + gridSize/2, midRow * gridSize + gridSize/2});
        }
    }
    
    // Agregar punto de salida
    simplePath.push_back({exit.x * gridSize + gridSize/2, exit.y * gridSize + gridSize/2});
    
    // Agregar este camino a la lista de caminos posibles
    paths.push_back(simplePath);
    
    // En una implementación completa, aquí generaríamos múltiples caminos
    // usando A* desde la entrada hasta la salida
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

void EnemyManager::processTowerAttacks(const std::vector<std::unique_ptr<Tower>>& towers) {
    // Este método sería llamado desde Game para gestionar los ataques de las torres a los enemigos
    // Por ahora es un stub, se implementará cuando integremos el sistema completo
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