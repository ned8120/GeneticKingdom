#ifndef TOWER_MANAGER_H
#define TOWER_MANAGER_H

#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "Tower.h"
#include "ResourceSystem.h"

enum class TowerType {
    NONE,
    ARCHER,
    MAGE,
    ARTILLERY
};

class TowerManager {
private:
    std::vector<std::unique_ptr<Tower>> towers;
    TowerType selectedType;
    Tower* selectedTower;
    ResourceSystem* resources;
    
    // Renders the tower selection UI
    void renderTowerMenu(SDL_Renderer* renderer) const;
    
public:
    TowerManager(ResourceSystem* res);
    
    // Crear una nueva torre en la posición especificada
    bool createTower(int row, int col);
    
    // Actualizar todas las torres
    void update(int deltaTime);
    
    // Renderizar todas las torres y la interfaz
    void render(SDL_Renderer* renderer, int gridSize) const;
    
    // Seleccionar un tipo de torre para colocar
    void selectTowerType(TowerType type);
    
    // Obtener el tipo de torre actualmente seleccionado
    TowerType getSelectedType() const { return selectedType; }
    
    // Intentar seleccionar una torre existente en una posición
    bool selectTowerAt(int row, int col);
    
    // Intentar mejorar la torre seleccionada
    bool upgradeSelectedTower();
    
    // Manejar clics de ratón en la interfaz
    bool handleMouseClick(int x, int y, int gridSize);
};

#endif // TOWER_MANAGER_H