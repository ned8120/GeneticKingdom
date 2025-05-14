#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               board(nullptr), resources(nullptr) {
}

Game::~Game() {
    clean();
}

bool Game::initialize() {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL no pudo inicializarse: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Crear ventana
    window = SDL_CreateWindow(
        "Genetic Kingdom",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "No se pudo crear la ventana: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Crear renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "No se pudo crear el renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Crear tablero (12x16 celdas)
    board = new GameBoard(12, 16);
    
    // Crear sistema de recursos
    resources = new ResourceSystem(100); // 100 de oro inicial
    
    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        
        // Control de FPS simple
        SDL_Delay(1000/60); // Aproximadamente 60 FPS
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running = false;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                // Intentar colocar torre
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Point gridPos = board->screenToGrid(mouseX, mouseY);
                
                if (board->isValidTowerPosition(gridPos.y, gridPos.x)) {
                    // Verificar si tenemos suficiente oro
                    if (resources->spendGold(TOWER_COST)) {
                        board->placeTower(gridPos.y, gridPos.x);
                        std::cout << "Torre colocada! Oro restante: " << resources->getGold() << std::endl;
                    } else {
                        std::cout << "No tienes suficiente oro! Necesitas: " << TOWER_COST << std::endl;
                    }
                }
            }
        }
    }
}

void Game::update() {
    // Futuras actualizaciones del estado del juego
}

void Game::render() {
    // Limpiar pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Dibujar tablero
    board->render(renderer);
    
    // Dibujar interfaz de usuario
    renderUI();
    
    // Actualizar pantalla
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    // Dibujar rectángulo para info de recursos
    SDL_Rect goldRect = {10, 10, 150, 30};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &goldRect);
    
    // Por ahora, solo imprimimos el oro en la consola
    // En una versión más avanzada, usaríamos SDL_ttf para renderizar texto
    std::cout << "Oro: " << resources->getGold() << std::endl;
}

void Game::clean() {
    // Liberar recursos
    delete board;
    delete resources;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}