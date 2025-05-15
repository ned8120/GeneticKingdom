#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

// Add this line - defines equality comparison for SDL_Point
inline bool operator==(const SDL_Point& a, const SDL_Point& b) {
    return a.x == b.x && a.y == b.y;
}

class AStar {
private:
    // Estructura para nodos en el algoritmo A*
    struct Node {
        SDL_Point position;  // Posición en la cuadrícula
        int gCost;           // Costo desde el inicio
        int hCost;           // Heurística (estimación hasta el destino)
        
        // Obtener costo total (f = g + h)
        int getFCost() const { return gCost + hCost; }
        
        // Constructor
        Node(SDL_Point pos, int g, int h) : position(pos), gCost(g), hCost(h) {}
        
        // Operadores de comparación para la cola de prioridad
        bool operator<(const Node& other) const {
            // Ordenar primero por costo F, luego por H para desempatar
            if (getFCost() == other.getFCost())
                return hCost > other.hCost;
            return getFCost() > other.getFCost();
        }
        
        bool operator==(const Node& other) const {
            return position.x == other.position.x && position.y == other.position.y;
        }
    };
    
    // Hash function para SDL_Point (para usar en unordered_set/map)
    struct PointHash {
        std::size_t operator()(const SDL_Point& point) const {
            return std::hash<int>()(point.x) ^ std::hash<int>()(point.y);
        }
    };
    
    struct PointEqual {
        bool operator()(const SDL_Point& a, const SDL_Point& b) const {
            return a.x == b.x && a.y == b.y;
        }
    };

public:
    // Rest of your AStar implementation...
    // [same as before]
    
    // Función principal para encontrar camino (devuelve vector de puntos)
    static std::vector<SDL_Point> findPath(
        const std::function<bool(int, int)>& isWalkable,
        SDL_Point start, SDL_Point end, int width, int height) {
        
        // Cola de prioridad para nodos por explorar (ordenados por menor costo F)
        std::priority_queue<Node> openSet;
        
        // Conjunto de nodos ya procesados
        std::unordered_set<SDL_Point, PointHash, PointEqual> closedSet;
        
        // Mapa para reconstruir el camino
        std::unordered_map<SDL_Point, SDL_Point, PointHash, PointEqual> cameFrom;
        
        // Mapa para almacenar costos G
        std::unordered_map<SDL_Point, int, PointHash, PointEqual> gScore;
        
        // Inicializar con el nodo de inicio
        openSet.push(Node(start, 0, calculateHCost(start, end)));
        gScore[start] = 0;
        
        // Direcciones posibles (4 direcciones: arriba, derecha, abajo, izquierda)
        const int dx[] = {0, 1, 0, -1};
        const int dy[] = {-1, 0, 1, 0};
        
        // Bucle principal de A*
        while (!openSet.empty()) {
            // Obtener nodo con menor costo F
            Node current = openSet.top();
            openSet.pop();
            
            // Si llegamos al destino
            if (current.position.x == end.x && current.position.y == end.y) {
                return reconstructPath(cameFrom, current.position);
            }
            
            // Marcar como procesado
            if (closedSet.find(current.position) != closedSet.end()) {
                continue;  // Si ya lo procesamos, saltamos
            }
            closedSet.insert(current.position);
            
            // Explorar vecinos
            for (int i = 0; i < 4; i++) {
                int newX = current.position.x + dx[i];
                int newY = current.position.y + dy[i];
                
                SDL_Point neighbor = {newX, newY};
                
                // Verificar límites y si es caminable
                if (newX < 0 || newX >= width || newY < 0 || newY >= height || 
                    !isWalkable(newX, newY) || 
                    closedSet.find(neighbor) != closedSet.end()) {
                    continue;
                }
                
                // Calcular nuevo costo G (distancia desde inicio)
                int tentativeGScore = gScore[current.position] + 1;
                
                // Si es un nodo nuevo o encontramos un camino mejor
                if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                    // Actualizar camino y costo
                    cameFrom[neighbor] = current.position;
                    gScore[neighbor] = tentativeGScore;
                    
                    // Añadir a la cola con costo actualizado
                    int hCost = calculateHCost(neighbor, end);
                    openSet.push(Node(neighbor, tentativeGScore, hCost));
                }
            }
        }
        
        // No se encontró camino
        return std::vector<SDL_Point>();
    }

private:
    // Heurística: distancia de Manhattan
    static int calculateHCost(SDL_Point a, SDL_Point b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
    
    // Reconstruir el camino desde el destino hasta el inicio
    static std::vector<SDL_Point> reconstructPath(
        const std::unordered_map<SDL_Point, SDL_Point, PointHash, PointEqual>& cameFrom,
        SDL_Point current) {
        
        std::vector<SDL_Point> path;
        
        // Añadir destino
        path.push_back(current);
        
        // Recorrer el mapa 'cameFrom' hasta llegar al inicio
        while (cameFrom.find(current) != cameFrom.end()) {
            current = cameFrom.at(current);
            path.push_back(current);
        }
        
        // Invertir para obtener camino desde inicio a fin
        std::reverse(path.begin(), path.end());
        
        return path;
    }
};

#endif // ASTAR_H