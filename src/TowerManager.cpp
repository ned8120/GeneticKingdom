#include "TowerManager.h"
#include "ArcherTower.h"
#include "MageTower.h"
#include "ArtilleryTower.h"
#include <iostream>

TowerManager::TowerManager(ResourceSystem* res, SDL_Renderer* renderer) 
    : selectedType(TowerType::NONE), selectedTower(nullptr), resources(res),
      archerTexture(nullptr), mageTexture(nullptr), artilleryTexture(nullptr) {
    // Cargar texturas
    loadTextures(renderer);
}

TowerManager::~TowerManager() {
    // Liberar texturas
    if (archerTexture) SDL_DestroyTexture(archerTexture);
    if (mageTexture) SDL_DestroyTexture(mageTexture);
    if (artilleryTexture) SDL_DestroyTexture(artilleryTexture);
}

bool TowerManager::loadTextures(SDL_Renderer* renderer) {
    // Cargar las imágenes y escalarlas
    SDL_Surface* surface = IMG_Load("images/arquero.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen arquero.png: " << IMG_GetError() << std::endl;
        return false;
    }
    archerTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("images/mago.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen mago.png: " << IMG_GetError() << std::endl;
        return false;
    }
    mageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("images/artillero.png");
    if (!surface) {
        std::cerr << "No se pudo cargar la imagen artillero.png: " << IMG_GetError() << std::endl;
        return false;
    }
    artilleryTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return archerTexture && mageTexture && artilleryTexture;
}

bool TowerManager::createTower(int row, int col) {
    // Comprobamos que haya un tipo seleccionado
    if (selectedType == TowerType::NONE) {
        return false;
    }
    
    // Determinar el costo basado en el tipo
    int cost = 0;
    switch (selectedType) {
        case TowerType::ARCHER:
            cost = 25; // Costo de torre de arqueros
            break;
        case TowerType::MAGE:
            cost = 50; // Costo de torre de magos
            break;
        case TowerType::ARTILLERY:
            cost = 75; // Costo de torre de artillería
            break;
        default:
            return false;
    }
    
    // Verificar si hay suficiente oro
    if (!resources->spendGold(cost)) {
        std::cout << "¡No hay suficiente oro para esta torre!" << std::endl;
        return false;
    }
    
    // Crear la torre del tipo seleccionado
    switch (selectedType) {
        case TowerType::ARCHER:
            towers.push_back(std::make_unique<ArcherTower>(row, col, archerTexture));
            break;
        case TowerType::MAGE:
            towers.push_back(std::make_unique<MageTower>(row, col, mageTexture));
            break;
        case TowerType::ARTILLERY:
            towers.push_back(std::make_unique<ArtilleryTower>(row, col, artilleryTexture));
            break;
        default:
            return false;
    }
    
    std::cout << "Torre creada: " << towers.back()->getType() << std::endl;
    return true;
}

void TowerManager::update(int deltaTime) {
    // Actualizar todas las torres
    for (auto& tower : towers) {
        tower->update(deltaTime);
    }
}

void TowerManager::render(SDL_Renderer* renderer, int gridSize) const {
    // Renderizar todas las torres
    for (const auto& tower : towers) {
        tower->render(renderer, gridSize);
    }
    
    // Renderizar interfaz de selección de torres
    renderTowerMenu(renderer);
    
    // Si hay una torre seleccionada, destacarla
    if (selectedTower) {
        // Encontrar la torre en el vector
        for (const auto& tower : towers) {
            if (tower.get() == selectedTower) {
                // Dibujar un borde blanco alrededor de la torre seleccionada
                SDL_Rect highlightRect = {
                    tower->getCol() * gridSize - 2, 
                    tower->getRow() * gridSize - 2, 
                    gridSize + 4, 
                    gridSize + 4
                };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &highlightRect);
                
                // Mostrar información sobre la torre
                // (En una versión más avanzada, esto sería texto renderizado)
                std::cout << "Torre seleccionada: " << tower->getTypeString() << std::endl;
                std::cout << "Daño: " << tower->getDamage() << std::endl;
                std::cout << "Alcance: " << tower->getRange() << std::endl;
                std::cout << "Velocidad: " << tower->getAttackSpeed() << "ms" << std::endl;
                
                // Mostrar costo de mejora si no está al nivel máximo
                if (tower->getLevel() < 3) {
                    std::cout << "Costo de mejora: " << tower->getUpgradeCost() << " oro" << std::endl;
                } else {
                    std::cout << "Nivel máximo alcanzado" << std::endl;
                }
                
                break;
            }
        }
    }
}

void TowerManager::renderTowerMenu(SDL_Renderer* renderer) const {
    // Panel superior para la selección de torres
    SDL_Rect menuRect = {0, 0, 160, 50};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &menuRect);
    
    // Botones para los tres tipos de torres
    SDL_Rect archerButton = {10, 10, 30, 30};
    SDL_Rect mageButton = {50, 10, 30, 30};
    SDL_Rect artilleryButton = {90, 10, 30, 30};
    
    // Dibujar los botones con las miniaturas de las texturas
    if (archerTexture) {
        SDL_RenderCopy(renderer, archerTexture, NULL, &archerButton);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Verde oscuro para arqueros
        SDL_RenderFillRect(renderer, &archerButton);
    }
    
    if (mageTexture) {
        SDL_RenderCopy(renderer, mageTexture, NULL, &mageButton);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255); // Azul para magos
        SDL_RenderFillRect(renderer, &mageButton);
    }
    
    if (artilleryTexture) {
        SDL_RenderCopy(renderer, artilleryTexture, NULL, &artilleryButton);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255); // Rojo oscuro para artilleros
        SDL_RenderFillRect(renderer, &artilleryButton);
    }
    
    // Dibujar bordes para el tipo seleccionado
    if (selectedType == TowerType::ARCHER) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &archerButton);
    }
    
    if (selectedType == TowerType::MAGE) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &mageButton);
    }
    
    if (selectedType == TowerType::ARTILLERY) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &artilleryButton);
    }
    
    // Dibujar botón de mejora cuando hay una torre seleccionada
    if (selectedTower) {
        SDL_Rect upgradeButton = {130, 10, 30, 30};
        
        // Fondo del botón - color dorado brillante
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderFillRect(renderer, &upgradeButton);
        
        // Borde del botón - más grueso para destacarlo
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &upgradeButton);
        
        // Dibujar un segundo borde para hacerlo más visible
        SDL_Rect outerBorder = {129, 9, 32, 32};
        SDL_RenderDrawRect(renderer, &outerBorder);
        
        // Dibujar símbolo de mejora (flecha hacia arriba)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        
        // Flecha hacia arriba
        SDL_RenderDrawLine(renderer, 145, 15, 145, 35); // Eje vertical
        SDL_RenderDrawLine(renderer, 137, 23, 145, 15); // Ala izquierda
        SDL_RenderDrawLine(renderer, 153, 23, 145, 15); // Ala derecha
        
        // Mostrar costo de mejora si no está al nivel máximo
        if (selectedTower->getLevel() < 3) {
            // Aquí se podría añadir texto renderizado con el costo
            // Por ahora solo mostramos en consola
            std::cout << "Costo de mejora: " << selectedTower->getUpgradeCost() << " oro" << std::endl;
        } else {
            // Indicar nivel máximo con una X
            SDL_RenderDrawLine(renderer, 135, 15, 155, 35);
            SDL_RenderDrawLine(renderer, 135, 35, 155, 15);
        }
    }
    
    // Mostrar precios (esto sería texto renderizado en una versión más avanzada)
    std::cout << "Arquero: 25 oro | Mago: 50 oro | Artillero: 75 oro" << std::endl;
}

void TowerManager::selectTowerType(TowerType type) {
    selectedType = type;
    selectedTower = nullptr; // Deseleccionar torre si hay alguna
    std::cout << "Tipo de torre seleccionado: ";
    
    switch (type) {
        case TowerType::ARCHER:
            std::cout << "Arquero" << std::endl;
            break;
        case TowerType::MAGE:
            std::cout << "Mago" << std::endl;
            break;
        case TowerType::ARTILLERY:
            std::cout << "Artillero" << std::endl;
            break;
        case TowerType::NONE:
            std::cout << "Ninguno" << std::endl;
            break;
    }
}

bool TowerManager::selectTowerAt(int row, int col) {
    for (auto& tower : towers) {
        if (tower->getRow() == row && tower->getCol() == col) {
            selectedTower = tower.get();
            selectedType = TowerType::NONE; // Desactivar selección de tipo
            return true;
        }
    }
    return false;
}

bool TowerManager::upgradeSelectedTower() {
    if (!selectedTower || selectedTower->getLevel() >= 3) {
        return false;
    }
    
    int cost = selectedTower->getUpgradeCost();
    if (!resources->spendGold(cost)) {
        std::cout << "No hay suficiente oro para mejorar la torre" << std::endl;
        return false;
    }
    
    if (selectedTower->upgrade()) {
        std::cout << "¡Torre mejorada a nivel " << selectedTower->getLevel() << "!" << std::endl;
        return true;
    }
    
    return false;
}

bool TowerManager::handleMouseClick(int x, int y, int gridSize) {
    // Verificar si el clic fue en el menú de selección de torres
    if (y < 50) { // Altura del menú
        if (x < 40) { // Botón de arquero
            selectTowerType(TowerType::ARCHER);
            return true;
        } else if (x < 80) { // Botón de mago
            selectTowerType(TowerType::MAGE);
            return true;
        } else if (x < 120) { // Botón de artillero
            selectTowerType(TowerType::ARTILLERY);
            return true;
        } else if (selectedTower && x > 130 && x < 160) { // Botón de mejora
            return upgradeSelectedTower();
        }
    }
    return false;
}