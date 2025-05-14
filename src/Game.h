#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "GameBoard.h"
#include "ResourceSystem.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    GameBoard* board;
    ResourceSystem* resources;
    
    // Constantes del juego
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    
    // Costos de torres
    const int TOWER_COST = 25;
    
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