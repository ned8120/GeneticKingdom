#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>  // Añadido para soporte de texto
#include <vector>
#include <string>
#include <deque>  // Para almacenar los mensajes recientes
#include "GameBoard.h"
#include "ResourceSystem.h"
#include "TowerManager.h"
#include "EnemyManager.h"

// Estructura para almacenar mensajes de ataque
struct AttackMessage {
    std::string text;
    int timeToLive;  // Tiempo que estará visible en milisegundos
    SDL_Color color;
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    GameBoard* board;
    ResourceSystem* resources;
    TowerManager* towerManager;
    EnemyManager* enemyManager;  // Nuevo: gestor de enemigos
    
    // Para renderizar texto
    TTF_Font* font;
    std::deque<AttackMessage> attackMessages;  // Lista de mensajes recientes
    const int MAX_MESSAGES = 5;  // Número máximo de mensajes mostrados
    
    // Tiempo para control de FPS y actualización
    Uint32 lastFrameTime;
    
    // Constantes del juego
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const int GRID_SIZE = 50;
    
    // Método para renderizar interfaz
    void renderUI();
    
    // Método para renderizar mensajes de ataque
    void renderAttackMessages();
    
    // Modo de test (para generar enemigos manualmente)
    bool testMode;
    
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
    
    // Añadir mensaje de ataque
    void addAttackMessage(const std::string& text, const SDL_Color& color);
};

#endif // GAME_H