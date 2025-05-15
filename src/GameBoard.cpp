#include "GameBoard.h"
#include <iostream>

const int GRID_SIZE = 50; // Tamaño de cada celda

GameBoard::GameBoard(int r, int c) : rows(r), cols(c) {
    // Inicializar el tablero con celdas vacías
    grid.resize(rows, std::vector<int>(cols, 0));
    
    // Definir entrada y salida
    entrance = {0, r/2};
    exit = {c-1, r/2};
    
    // Inicializar el mapa con múltiples caminos
    initializeMap();
}

void GameBoard::initializeMap() {
    // Limpiamos el grid (excepto las torres)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r][c] != 2) { // No tocar las torres existentes
                grid[r][c] = 0;
            }
        }
    }
    
    // Camino principal en medio (como antes)
    for (int c = 0; c < cols; c++) {
        grid[rows/2][c] = 1;
    }
    
    // Camino alternativo por arriba
    if (rows > 4) {
        for (int c = 0; c < cols/2; c++) {
            grid[rows/4][c] = 1;
        }
        // Conectar con el camino principal
        for (int r = rows/4; r <= rows/2; r++) {
            grid[r][cols/2-1] = 1;
        }
    }
    
    // Camino alternativo por abajo
    if (rows > 5) {
        for (int c = cols/2; c < cols; c++) {
            grid[3*rows/4][c] = 1;
        }
        // Conectar con el camino principal
        for (int r = rows/2; r <= 3*rows/4; r++) {
            grid[r][cols/2] = 1;
        }
    }
    
    // Marcar entrada y salida
    grid[entrance.y][entrance.x] = 1;
    grid[exit.y][exit.x] = 1;
}

bool GameBoard::hasValidPath() const {
    return findPath(entrance, exit);
}

bool GameBoard::findPath(SDL_Point start, SDL_Point end) const {
    // Implementación de BFS para encontrar camino
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<SDL_Point> queue;
    
    // Agregar punto de inicio
    queue.push(start);
    visited[start.y][start.x] = true;
    
    // Movimientos posibles (arriba, derecha, abajo, izquierda)
    const int dr[] = {-1, 0, 1, 0};
    const int dc[] = {0, 1, 0, -1};
    
    while (!queue.empty()) {
        SDL_Point current = queue.front();
        queue.pop();
        
        // Si llegamos al final, hay un camino
        if (current.x == end.x && current.y == end.y) {
            return true;
        }
        
        // Verificar vecinos
        for (int i = 0; i < 4; i++) {
            int newR = current.y + dr[i];
            int newC = current.x + dc[i];
            
            // Comprobar límites y si es un camino no visitado
            if (newR >= 0 && newR < rows && newC >= 0 && newC < cols && 
                grid[newR][newC] == 1 && !visited[newR][newC]) {
                SDL_Point next = {newC, newR};
                queue.push(next);
                visited[newR][newC] = true;
            }
        }
    }
    
    // No se encontró camino
    return false;
}

void GameBoard::render(SDL_Renderer* renderer) const {
    // Dibujar el tablero
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            SDL_Rect cell = {c * GRID_SIZE, r * GRID_SIZE, GRID_SIZE, GRID_SIZE};
            
            // Colorear según el tipo de celda
            switch (grid[r][c]) {
                case 0: // Celda vacía - verde claro
                    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255);
                    break;
                case 1: // Camino - marrón
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                    break;
                case 2: // Torre - gris
                    SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
                    break;
            }
            
            SDL_RenderFillRect(renderer, &cell);
            
            // Dibujar borde de la celda
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
    
    // Dibujar entrada (izquierda) - rojo
    SDL_Rect entranceRect = {entrance.x * GRID_SIZE, entrance.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &entranceRect);
    
    // Dibujar puente/salida (derecha) - azul
    SDL_Rect exitRect = {exit.x * GRID_SIZE, exit.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &exitRect);
}

bool GameBoard::isValidTowerPosition(int r, int c) const {
    // Verificar límites
    if (r < 0 || r >= rows || c < 0 || c >= cols) {
        return false;
    }
    
    // No permitir colocar torres en caminos
    if (grid[r][c] != 0) {
        return false;
    }
    
    // Simular colocación para verificar si bloquearía todos los caminos
    std::vector<std::vector<int>> tempGrid = grid;
    tempGrid[r][c] = 2; // Colocar torre temporalmente
    
    // Crear tablero temporal para verificar camino
    GameBoard tempBoard = *this;
    tempBoard.grid = tempGrid;
    
    // Verificar si hay un camino válido después de colocar la torre
    return tempBoard.hasValidPath();
}

bool GameBoard::placeTower(int r, int c) {
    if (isValidTowerPosition(r, c)) {
        grid[r][c] = 2;
        return true;
    }
    return false;
}

SDL_Point GameBoard::screenToGrid(int x, int y) const {
    return {x / GRID_SIZE, y / GRID_SIZE};
}

int GameBoard::getCellType(int r, int c) const {
    if (r >= 0 && r < rows && c >= 0 && c < cols) {
        return grid[r][c];
    }
    return -1; // Fuera de límites
}

bool GameBoard::isCellWalkable(int x, int y) const {
    // Verificar límites
    if (x < 0 || x >= cols || y < 0 || y >= rows) {
        return false;
    }
    
    // Celda es caminable si es un camino (1) o entrada/salida
    return grid[y][x] == 1 || 
           (x == entrance.x && y == entrance.y) || 
           (x == exit.x && y == exit.y);
}