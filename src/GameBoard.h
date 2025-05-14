#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include <utility>

class GameBoard {
private:
    int rows;
    int cols;
    std::vector<std::vector<int>> grid; // 0 = vacío, 1 = camino, 2 = torre
    
    // Posiciones de entrada y salida (puente)
    SDL_Point entrance;
    SDL_Point exit;
    
    // Verifica si hay al menos un camino disponible de entrada a salida
    bool hasValidPath() const;
    
    // Método para encontrar camino usando BFS (Breadth-First Search)
    bool findPath(SDL_Point start, SDL_Point end) const;

public:
    GameBoard(int r, int c);
    
    // Inicializa el tablero con múltiples caminos
    void initializeMap();
    
    // Dibuja el tablero
    void render(SDL_Renderer* renderer) const;
    
    // Verifica si es válido colocar una torre en (r, c)
    bool isValidTowerPosition(int r, int c) const;
    
    // Coloca una torre en (r, c) si es válido
    bool placeTower(int r, int c);
    
    // Convierte coordenadas de pantalla a coordenadas de grid
    SDL_Point screenToGrid(int x, int y) const;
    
    // Obtiene el número de filas y columnas
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    
    // Obtiene el tipo de celda en una posición
    int getCellType(int r, int c) const;
};

#endif // GAMEBOARD_H