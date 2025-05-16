#ifndef TOWER_H
#define TOWER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>  // For std::cout and std::endl

class Enemy;  // This tells the compiler "Enemy is a class that will be defined elsewhere"


// Clase base abstracta para todas las torres
class Tower {
protected:
    int level;          // Nivel actual de la torre (1-3)
    int damage;         // Daño base
    int range;          // Alcance en píxeles
    int attackSpeed;    // Velocidad de ataque (ms entre ataques)
    int specialCooldown; // Tiempo de regeneración del poder especial
    int row, col;       // Posición en el tablero
    int cost;           // Costo inicial de la torre
    int upgradeCost;    // Costo base para mejorar

    // Textura de la torre
    SDL_Texture* texture;
    
    // Color para renderizar la torre (como respaldo)
    SDL_Color color;
    
    // Tiempo transcurrido desde el último ataque
    int attackTimer;

     // Variables para ataques especiales
    float specialAttackProbability;  // Probabilidad de ataque especial (0.0-1.0)
    int specialAttackTimer;         // Temporizador para el ataque especial
    bool specialAttackReady;        // Indica si el ataque especial está listo

public:
    Tower(int r, int c, int initialCost, SDL_Texture* tex);
    virtual ~Tower();
    
    // Métodos comunes a todas las torres
    virtual void render(SDL_Renderer* renderer, int gridSize) const;
    virtual void update(int deltaTime);
    
    // Método virtual puro - cada tipo de torre debe implementar su propia lógica de ataque
    virtual void attack() = 0;
    
    // Método para mejorar la torre
    virtual bool upgrade();


    // Comprobar si la torre puede atacar ahora (basado en timer)
    bool canAttack() const { return attackTimer >= attackSpeed; }

    // Métodos para visualización de ataque y reseteo de temporizador
    void visualAttack() {
        attack(); // Solo para efectos visuales/sonido
    }
    
    void resetTimer() {
        attackTimer = 0;
    }
    
    // Métodos para obtener información
    int getLevel() const { return level; }
    int getDamage() const { return damage; }
    int getRange() const { return range; }
    int getAttackSpeed() const { return attackSpeed; }
    int getSpecialCooldown() const { return specialCooldown; }
    int getUpgradeCost() const;
    int getRow() const { return row; }
    int getCol() const { return col; }
    std::string getTypeString() const;
    
    // Método virtual para obtener el tipo específico
    virtual std::string getType() const = 0;


    // Métodos para ataques especiales
    virtual bool trySpecialAttack();
    virtual void performSpecialAttack() = 0;  // Método virtual puro
    
    // Getters para ataques especiales
    float getSpecialAttackProbability() const { return specialAttackProbability; }
    bool isSpecialAttackReady() const { return specialAttackReady; }
    
    // Método para visualizar el ataque especial (para efectos)
    void visualSpecialAttack();
};

#endif // TOWER_H