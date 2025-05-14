#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

class ResourceSystem {
private:
    int gold;
    
public:
    ResourceSystem(int initialGold = 100);
    
    // Obtener oro actual
    int getGold() const;
    
    // Añadir oro (por matar enemigos)
    void addGold(int amount);
    
    // Reducir oro (comprar torres)
    bool spendGold(int amount);
};

#endif // RESOURCE_SYSTEM_H