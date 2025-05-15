#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

// Forward declaration - this tells the compiler that GameBoard exists
// without needing to include the full header
class GameBoard;

class Enemy {
protected:
    // Atributos básicos
    int health;               // Puntos de vida
    float speed;              // Velocidad de movimiento (píxeles por segundo)
    float arrowResistance;    // Resistencia a las flechas (0.0 - 1.0)
    float magicResistance;    // Resistencia a la magia (0.0 - 1.0)
    float artilleryResistance; // Resistencia a la artillería (0.0 - 1.0)
    
    // Posición y movimiento
    float x, y;               // Posición actual (usar float para movimiento suave)
    int currentPathIndex;     // Índice actual en el camino
    std::vector<SDL_Point> path; // Camino a seguir
    bool reachedEnd;          // Si el enemigo ha llegado al final
    
    // Apariencia
    SDL_Texture* texture;     // Textura del enemigo
    SDL_Rect destRect;        // Rectángulo de destino para renderizar
    int size;                 // Tamaño del enemigo (ancho/alto)
    
    // Valor al morir
    int goldValue;            // Cantidad de oro que da al morir

    // Referencia al tablero para recalcular caminos
    GameBoard* gameBoard = nullptr;


    int id;                    // ID único para el enemigo
    float damageDealt;         // Daño total causado al jugador

public:
    Enemy(float startX, float startY, const std::vector<SDL_Point>& pathPoints, SDL_Texture* tex, int enemySize = 40);
    virtual ~Enemy();
    
    // Métodos principales
    virtual void update(int deltaTime);
    virtual void render(SDL_Renderer* renderer) const;
    
    // Recibir daño de diferentes tipos de torres
    virtual int takeDamage(int damage, std::string towerType);
    
    // Métodos para obtener información
    bool isAlive() const { return health > 0; }
    bool hasReachedEnd() const { return reachedEnd; }
    int getGoldValue() const { return goldValue; }
    SDL_Point getPosition() const { return {static_cast<int>(x), static_cast<int>(y)}; }
    
    // Método para obtener la salud actual
    int getHealth() const { return health; }
    
    // Método para calcular la distancia a un punto
    float distanceTo(int targetX, int targetY) const;
    
    // Métodos para establecer y obtener el camino
    void setPath(const std::vector<SDL_Point>& newPath);
    
    // Método virtual para obtener el tipo específico
    virtual std::string getType() const = 0;

    // Método para obtener el progreso en el camino
    float getPathProgress() const { return static_cast<float>(currentPathIndex) / path.size(); }

    // Recalcular camino usando A*
    void recalculatePath(GameBoard* board);

    // Establecer el tablero para recálculos de camino
    void setGameBoard(GameBoard* board) { gameBoard = board; }

    // Getters y setters para ID
    void setId(int enemyId) { id = enemyId; }
    int getId() const { return id; }
        
    // Getters y setters para estadísticas genéticas
    void setHealth(int newHealth) { health = newHealth; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setArrowResistance(float value) { arrowResistance = value; }
    void setMagicResistance(float value) { magicResistance = value; }
    void setArtilleryResistance(float value) { artilleryResistance = value; }
        
    // Estadísticas para fitness
    float getDamageDealt() const { return damageDealt; }
    void addDamageDealt(float damage) { damageDealt += damage; }
};

#endif // ENEMY_H