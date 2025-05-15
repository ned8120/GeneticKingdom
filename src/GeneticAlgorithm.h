#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <memory>
#include "Enemy.h"

struct Genome {
    float health;
    float speed;
    float arrowResistance;
    float magicResistance;
    float artilleryResistance;
    int enemyType;  // 0=Ogro, 1=Elfo Oscuro, 2=Harpía, 3=Mercenario
    float fitness;

    Genome() : health(0.0f), speed(0.0f), arrowResistance(0.0f), 
               magicResistance(0.0f), artilleryResistance(0.0f),
               enemyType(0), fitness(0.0f) {}
};

class GeneticAlgorithm {
private:
    std::vector<Genome> population;
    std::mt19937 rng;
    
    // Configuración del algoritmo genético
    int populationSize;
    float mutationRate;
    float crossoverRate;
    int eliteCount;
    int currentGeneration;
    
    // Métricas para estadísticas
    int mutationsOccurred;
    float averageFitness;
    float bestFitness;
    float worstFitness;
    
    // Límites para los atributos
    struct AttributeLimits {
        float minHealth, maxHealth;
        float minSpeed, maxSpeed;
        float minResistance, maxResistance;
    } limits;

    // Métodos privados
    void initializePopulation();
    std::vector<Genome> selectParents();
    Genome crossover(const Genome& parent1, const Genome& parent2);
    void mutate(Genome& genome);
    void updateStatistics();

public:
    GeneticAlgorithm(int populationSize = 20, float mutationRate = 0.1f, 
                     float crossoverRate = 0.7f, int eliteCount = 2);
    
    // Evolucionar la población
    void evolve();
    
    // Inicializar los genomas según los tipos de enemigos
    void initializeGenome(Genome& genome, int enemyType);
    
    // Actualizar el fitness de un genoma basado en el rendimiento del enemigo
    void updateFitness(Genome& genome, float progressMade, float damageDealt, float timeAlive);
    
    // Obtener genomas para crear enemigos
    const std::vector<Genome>& getPopulation() const { return population; }
    
    // Getters para estadísticas
    int getGeneration() const { return currentGeneration; }
    int getMutationsOccurred() const { return mutationsOccurred; }
    float getAverageFitness() const { return averageFitness; }
    float getBestFitness() const { return bestFitness; }
    float getWorstFitness() const { return worstFitness; }
    float getMutationRate() const { return mutationRate; }
    
    // Actualizar la configuración
    void setMutationRate(float rate) { mutationRate = rate; }

    std::vector<Genome>& getPopulationRef() { return population; }

};

#endif // GENETIC_ALGORITHM_H