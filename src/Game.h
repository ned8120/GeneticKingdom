#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GameBoard.h"
#include "ResourceSystem.h"
#include "TowerManager.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    GameBoard* board;
    ResourceSystem* resources;
    TowerManager* towerManager;
    
    // Tiempo para control de FPS y actualización
    Uint32 lastFrameTime;
    
    // Constantes del juego
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const int GRID_SIZE = 50;
    
    // Método para renderizar interfaz
    void renderUI();
    
public:
    Game();
    ~Game();
    
    // Inicializa el juego
    bool initialize();
    
    // Bucle principal
    void run();
    
    // Maneja eventos
    void handleEvents();
    
    // Actualiza el estado del juego
    void update();
    
    // Renderiza todo
    void render();
    
    // Limpia recursos
    void clean();
};

#endif // GAME_H