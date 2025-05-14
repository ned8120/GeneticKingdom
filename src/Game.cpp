#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               board(nullptr), resources(nullptr), towerManager(nullptr),
               enemyManager(nullptr), lastFrameTime(0), testMode(true) {
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
    
    // Inicializar SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image no pudo inicializarse: " << IMG_GetError() << std::endl;
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
    
    // Crear gestor de torres
    towerManager = new TowerManager(resources, renderer);
    
    // Crear gestor de enemigos
    enemyManager = new EnemyManager(board, resources, renderer);
    
    // Inicializar tiempo
    lastFrameTime = SDL_GetTicks();
    
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
        } else if (e.type == SDL_KEYDOWN) {
            // Teclas para pruebas
            if (e.key.keysym.sym == SDLK_SPACE && testMode) {
                enemyManager->spawnTestEnemies();
            } else if (e.key.keysym.sym == SDLK_w && testMode) {
                enemyManager->spawnWave();
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                
                // Primero verificar si se hizo clic en la interfaz
                if (towerManager->handleMouseClick(mouseX, mouseY, GRID_SIZE)) {
                    continue; // El clic fue manejado por la interfaz
                }
                
                // Convertir a coordenadas de grid
                SDL_Point gridPos = board->screenToGrid(mouseX, mouseY);
                
                // Comprobar si hay una torre en esa posición
                if (towerManager->selectTowerAt(gridPos.y, gridPos.x)) {
                    std::cout << "Torre seleccionada en (" 
                              << gridPos.y << "," << gridPos.x << ")" << std::endl;
                    continue;
                }
                
                // Si hay un tipo de torre seleccionado y la posición es válida
                if (towerManager->getSelectedType() != TowerType::NONE &&
                    board->isValidTowerPosition(gridPos.y, gridPos.x)) {
                    
                    // Intentar crear torre
                    if (towerManager->createTower(gridPos.y, gridPos.x)) {
                        board->placeTower(gridPos.y, gridPos.x);
                        std::cout << "Torre colocada en (" 
                                  << gridPos.y << "," << gridPos.x 
                                  << "). Oro restante: " << resources->getGold() << std::endl;
                    }
                }
            }
        }
    }
}

void Game::update() {
    // Calcular tiempo transcurrido desde el último frame
    Uint32 currentTime = SDL_GetTicks();
    int deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    
    // Actualizar torres
    towerManager->update(deltaTime);
    
    // Actualizar enemigos
    enemyManager->update(deltaTime);
    
    // Procesar ataques de torres a enemigos (se implementará después)
    // enemyManager->processTowerAttacks(towerManager->getTowers());
}

void Game::render() {
    // Limpiar pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Dibujar tablero
    board->render(renderer);
    
    // Dibujar torres
    towerManager->render(renderer, GRID_SIZE);
    
    // Dibujar enemigos
    enemyManager->render(renderer);
    
    // Dibujar interfaz de usuario
    renderUI();
    
    // Actualizar pantalla
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    // Dibujar rectángulo para info de recursos
    SDL_Rect goldRect = {SCREEN_WIDTH - 160, 10, 150, 30};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &goldRect);
    
    // Por ahora, solo imprimimos el oro en la consola
    std::cout << "Oro: " << resources->getGold() << std::endl;
    
    // Información sobre oleadas (para la fase 3)
    SDL_Rect waveRect = {SCREEN_WIDTH - 160, 50, 150, 30};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &waveRect);
    
    // Mostrar instrucciones de prueba
    if (testMode) {
        std::cout << "ESPACIO: generar enemigos de prueba | W: generar oleada" << std::endl;
        std::cout << "Oleada actual: " << enemyManager->getCurrentWave() << std::endl;
        std::cout << "Enemigos activos: " << enemyManager->getEnemyCount() << std::endl;
    }
}

void Game::clean() {
    // Liberar recursos
    delete board;
    delete resources;
    delete towerManager;
    delete enemyManager;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}