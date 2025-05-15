#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <memory>
#include <random>
#include "Enemy.h"
#include "GameBoard.h"
#include "ResourceSystem.h"
#include "AStar.h"



// Forward declaration de la clase Tower
class Tower;
class Game;  // Añadir esta línea

enum class EnemyType {
    OGRE,
    DARK_ELF,
    HARPY,
    MERCENARY
};

class EnemyManager {
private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::vector<SDL_Point>> paths;  // Caminos posibles
    
    // Texturas para cada tipo de enemigo
    SDL_Texture* ogreTexture;
    SDL_Texture* darkElfTexture;
    SDL_Texture* harpyTexture;
    SDL_Texture* mercenaryTexture;
    
    // Punto de entrada (inicio del camino)
    SDL_Point entrancePoint;
    
    // Sistema de recursos para añadir oro al matar enemigos
    ResourceSystem* resources;
    
    // Temporizador para oleadas
    int waveTimer;
    int waveInterval;    // Tiempo entre oleadas (ms)
    int enemiesPerWave;  // Enemigos por oleada
    int currentWave;     // Número de oleada actual
    
    // Generador de números aleatorios
    std::mt19937 rng;
    std::uniform_int_distribution<int> enemyTypeDist;
    std::uniform_int_distribution<int> pathDist;
    
    // Crear un nuevo enemigo según el tipo
    std::unique_ptr<Enemy> createEnemy(EnemyType type, const std::vector<SDL_Point>& path);
    
    // Genera caminos usando A*
    void generatePathsWithAStar(GameBoard* board);

    // Genera caminos alternativos
    void generateAlternativePaths(GameBoard* board, SDL_Point entrance, SDL_Point exit);

    // Verifica si un camino es suficientemente diferente de los existentes
    bool isPathSufficientlyDifferent(const std::vector<SDL_Point>& newPath);


public:
    EnemyManager(GameBoard* board, ResourceSystem* res, SDL_Renderer* renderer);
    ~EnemyManager();
    
    // Cargar texturas para los enemigos
    bool loadTextures(SDL_Renderer* renderer);
    
    // Generar los caminos posibles desde el mapa
    void generatePaths(GameBoard* board);
    
    // Iniciar una nueva oleada de enemigos
    void spawnWave();
    
    // Actualizar todos los enemigos
    void update(int deltaTime);
    
    // Renderizar todos los enemigos
    void render(SDL_Renderer* renderer) const;
    
    // Comprobar colisiones y daño desde torres
    void processTowerAttacks(const std::vector<std::unique_ptr<Tower>>& towers, Game* game = nullptr);
    
    // Obtener estadísticas
    int getEnemyCount() const { return enemies.size(); }
    int getEnemiesKilled() const;  // Implementar para estadísticas
    int getCurrentWave() const { return currentWave; }
    
    // Método para pruebas: generar un enemigo de cada tipo
    void spawnTestEnemies();
};

#endif // ENEMY_MANAGER_H