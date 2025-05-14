#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 50; // Tamaño de cada celda del tablero

// Clase para manejar el tablero del juego
class GameBoard {
private:
    int rows;
    int cols;
    std::vector<std::vector<int>> grid; // 0 = vacío, 1 = camino, 2 = torre

public:
    GameBoard(int r, int c) : rows(r), cols(c) {
        // Inicializar el tablero con celdas vacías
        grid.resize(rows, std::vector<int>(cols, 0));
        
        // Definir un camino simple para demostración
        for (int i = 0; i < cols; i++) {
            grid[rows/2][i] = 1; // Camino horizontal en el medio
        }
    }

    void render(SDL_Renderer* renderer) {
        // Dibujar el tablero
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                SDL_Rect cell = {c * GRID_SIZE, r * GRID_SIZE, GRID_SIZE, GRID_SIZE};
                
                // Colorear según el tipo de celda
                if (grid[r][c] == 0) {
                    // Celda vacía - verde claro
                    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255);
                } else if (grid[r][c] == 1) {
                    // Camino - marrón
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                } else if (grid[r][c] == 2) {
                    // Torre - gris
                    SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
                }
                
                SDL_RenderFillRect(renderer, &cell);
                
                // Dibujar borde de la celda
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &cell);
            }
        }
        
        // Dibujar entrada (izquierda) y puente (derecha)
        SDL_Rect entrance = {0, rows/2 * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &entrance);
        
        SDL_Rect bridge = {(cols-1) * GRID_SIZE, rows/2 * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &bridge);
    }
    
    bool isValidTowerPosition(int r, int c) {
        // No permitir colocar torres en el camino
        return r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] == 0;
    }
    
    void placeTower(int r, int c) {
        if (isValidTowerPosition(r, c)) {
            grid[r][c] = 2;
        }
    }
};

int main(int argc, char* argv[]) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL no pudo inicializarse: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    // Crear ventana
    SDL_Window* window = SDL_CreateWindow(
        "Genetic Kingdom",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "No se pudo crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    
    // Crear renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "No se pudo crear el renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Crear tablero (12x16 celdas)
    GameBoard board(12, 16);
    
    // Loop principal
    bool quit = false;
    SDL_Event e;
    
    while (!quit) {
        // Manejar eventos
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Colocar una torre al hacer clic
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int gridRow = mouseY / GRID_SIZE;
                int gridCol = mouseX / GRID_SIZE;
                board.placeTower(gridRow, gridCol);
            }
        }
        
        // Limpiar pantalla
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Dibujar tablero
        board.render(renderer);
        
        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }
    
    // Liberar recursos
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
