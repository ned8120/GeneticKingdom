#include "Game.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Game::Game() : window(nullptr), renderer(nullptr), running(false), 
               board(nullptr), resources(nullptr), towerManager(nullptr),
               enemyManager(nullptr), lastFrameTime(0), testMode(true),
               font(nullptr) {
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
    
    // Inicializar SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf no pudo inicializarse: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Cargar fuente (asegúrate de que exista esta ruta)
    font = TTF_OpenFont("fonts/arial.ttf", 16);
    if (!font) {
        std::cerr << "No se pudo cargar la fuente: " << TTF_GetError() << std::endl;
        // Continuar sin fuente, mostraremos mensajes en consola
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

void Game::addAttackMessage(const std::string& text, const SDL_Color& color) {
    AttackMessage message;
    message.text = text;
    message.timeToLive = 2000;  // 2 segundos
    message.color = color;
    
    attackMessages.push_front(message);  // Añadir al principio
    
    // Limitar número de mensajes
    if (attackMessages.size() > MAX_MESSAGES) {
        attackMessages.pop_back();  // Eliminar el más antiguo
    }
    
    // También mostrar en consola por si acaso
    std::cout << text << std::endl;
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
    
    // Procesar ataques (pasar puntero a Game para los mensajes)
    std::cout << "Checking tower attacks..." << std::endl;
    enemyManager->processTowerAttacks(towerManager->getTowers(), this);
    
    // Actualizar tiempo de vida de los mensajes
    for (auto it = attackMessages.begin(); it != attackMessages.end(); ) {
        it->timeToLive -= deltaTime;
        if (it->timeToLive <= 0) {
            it = attackMessages.erase(it);
        } else {
            ++it;
        }
    }
    
    // Debug: Print current game state
    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0) {  // Print every 60 frames
        std::cout << "Game State: " 
                  << enemyManager->getEnemyCount() << " enemies active, "
                  << "Player gold: " << resources->getGold() << std::endl;
    }
}

void Game::renderAttackMessages() {
    // Si no tenemos fuente, no renderizar nada
    if (!font) return;
    
    int y = 50;  // Comenzar desde arriba
    
    for (const auto& msg : attackMessages) {
        SDL_Surface* surface = TTF_RenderText_Blended(font, msg.text.c_str(), msg.color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect destRect = {SCREEN_WIDTH - surface->w - 10, y, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &destRect);
                SDL_DestroyTexture(texture);
                y += surface->h + 5;  // Espacio entre mensajes
            }
            SDL_FreeSurface(surface);
        }
    }
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
    
    // Dibujar mensajes de ataque
    renderAttackMessages();
    
    // Actualizar pantalla
    SDL_RenderPresent(renderer);
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
                        
                        // MODIFICACIÓN PARA A*: Regenerar caminos cuando se coloca una torre
                        // Esto hace que los enemigos recalculen sus rutas cuando hay un nuevo obstáculo
                        enemyManager->generatePaths(board);
                    }
                }
            }
        }
    }
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

void Game::renderUI() {
    // Dibujar rectángulo para info de recursos
    SDL_Rect goldRect = {SCREEN_WIDTH - 160, 10, 150, 30};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &goldRect);
    
    // Si tenemos fuente, mostrar el oro en pantalla
    if (font) {
        std::string goldText = "Oro: " + std::to_string(resources->getGold());
        SDL_Surface* surface = TTF_RenderText_Blended(font, goldText.c_str(), {255, 215, 0, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {SCREEN_WIDTH - 150, 15, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    } else {
        // Por ahora, solo imprimimos el oro en la consola
        std::cout << "Oro: " << resources->getGold() << std::endl;
    }
    
    // Información sobre oleadas (para la fase 3)
    SDL_Rect waveRect = {SCREEN_WIDTH - 160, 50, 150, 30};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(renderer, &waveRect);
    
    // Mostrar información sobre oleadas si tenemos fuente
    if (font) {
        std::string waveText = "Oleada: " + std::to_string(enemyManager->getCurrentWave());
        SDL_Surface* surface = TTF_RenderText_Blended(font, waveText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {SCREEN_WIDTH - 150, 55, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
    
    // Mostrar instrucciones de prueba
    if (testMode) {
        std::cout << "ESPACIO: generar enemigos de prueba | W: generar oleada" << std::endl;
        std::cout << "Oleada actual: " << enemyManager->getCurrentWave() << std::endl;
        std::cout << "Enemigos activos: " << enemyManager->getEnemyCount() << std::endl;
    }
    
    // Mostrar estadísticas genéticas
    if (font) {
        SDL_Rect geneticRect = {10, SCREEN_HEIGHT - 140, 400, 130};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
        SDL_RenderFillRect(renderer, &geneticRect);
        
        int textY = SCREEN_HEIGHT - 135;
        int lineHeight = 18;
        
        // Línea 1: Generación actual
        std::string genText = "Generacion: " + std::to_string(enemyManager->getCurrentGeneration());
        SDL_Surface* surface = TTF_RenderText_Blended(font, genText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {15, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        textY += lineHeight;
        
        // Línea 2: Oleada actual
        std::string waveText = "Oleada: " + std::to_string(enemyManager->getCurrentWave());
        surface = TTF_RenderText_Blended(font, waveText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {15, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        textY += lineHeight;
        
        // Línea 3: Fitness - VERSIÓN CORREGIDA
        char fitnessBuffer[100];
        float avgFitness = enemyManager->getAverageFitness();
        float bestFitness = enemyManager->getBestFitness();
        float worstFitness = enemyManager->getWorstFitness();
        
        // Formatear con precisión de 2 decimales
        sprintf(fitnessBuffer, "Fitness: Prom=%.2f Mejor=%.2f Peor=%.2f", 
                avgFitness, bestFitness, worstFitness);
        std::string fitnessText = fitnessBuffer;
        
        surface = TTF_RenderText_Blended(font, fitnessText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {15, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        textY += lineHeight;
        
        // Línea 4: Mutaciones
        std::string mutText = "Tasa de mutacion: " + std::to_string(enemyManager->getMutationRate()).substr(0, 4) + 
                              " Mutaciones: " + std::to_string(enemyManager->getMutationsOccurred());
        surface = TTF_RenderText_Blended(font, mutText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {15, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        textY += lineHeight;
        
        // Línea 5: Enemigos activos
        std::string enemyText = "Enemigos activos: " + std::to_string(enemyManager->getEnemyCount());
        surface = TTF_RenderText_Blended(font, enemyText.c_str(), {255, 255, 255, 255});
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = {15, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void Game::clean() {
    // Liberar recursos
    delete board;
    delete resources;
    delete towerManager;
    delete enemyManager;
    
    // Liberar recursos de texto
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}