#include "GeneticAlgorithm.h"
#include <numeric>
#include <algorithm>

GeneticAlgorithm::GeneticAlgorithm(int popSize, float mutRate, 
                                 float crossRate, int elite)
    : populationSize(popSize), mutationRate(mutRate), 
      crossoverRate(crossRate), eliteCount(elite),
      currentGeneration(0), mutationsOccurred(0),
      averageFitness(0.0f), bestFitness(0.0f), worstFitness(0.0f) {
    
    // Inicializar el generador de números aleatorios
    std::random_device rd;
    rng = std::mt19937(rd());
    
    // Definir límites para los atributos
    limits.minHealth = 50.0f;
    limits.maxHealth = 200.0f;
    limits.minSpeed = 20.0f;
    limits.maxSpeed = 70.0f;
    limits.minResistance = 0.0f;
    limits.maxResistance = 0.9f;
    
    // Inicializar la población
    initializePopulation();
}

void GeneticAlgorithm::initializePopulation() {
    population.clear();
    population.resize(populationSize);
    
    // Distribuir los tipos de enemigos uniformemente en la población inicial
    for (int i = 0; i < populationSize; i++) {
        int enemyType = i % 4; // 0=Ogro, 1=Elfo, 2=Harpía, 3=Mercenario
        initializeGenome(population[i], enemyType);
    }
    
    currentGeneration = 1;
    mutationsOccurred = 0;
    
    std::cout << "Población inicial creada con " << populationSize << " individuos" << std::endl;
}

void GeneticAlgorithm::initializeGenome(Genome& genome, int enemyType) {
    genome.enemyType = enemyType;
    
    // Inicializar atributos según el tipo de enemigo
    switch (enemyType) {
        case 0: // Ogro
            genome.health = 150.0f;
            genome.speed = 20.0f;
            genome.arrowResistance = 0.7f;
            genome.magicResistance = 0.2f;
            genome.artilleryResistance = 0.3f;
            break;
        case 1: // Elfo Oscuro
            genome.health = 80.0f;
            genome.speed = 60.0f;
            genome.arrowResistance = 0.2f;
            genome.magicResistance = 0.7f;
            genome.artilleryResistance = 0.3f;
            break;
        case 2: // Harpía
            genome.health = 70.0f;
            genome.speed = 45.0f;
            genome.arrowResistance = 0.3f;
            genome.magicResistance = 0.3f;
            genome.artilleryResistance = 1.0f;
            break;
        case 3: // Mercenario
            genome.health = 100.0f;
            genome.speed = 35.0f;
            genome.arrowResistance = 0.6f;
            genome.magicResistance = 0.2f;
            genome.artilleryResistance = 0.6f;
            break;
    }
    
    // Fitness inicial a 0
    genome.fitness = 0.0f;
}

void GeneticAlgorithm::updateFitness(Genome& genome, float progressMade, float damageDealt, float timeAlive) {
    // Imprimir valores de entrada para depuración
    std::cout << "DEBUG - UpdateFitness con valores: progreso=" << progressMade 
              << ", daño=" << damageDealt 
              << ", tiempo=" << timeAlive << std::endl;
    
    // Verifica que al menos uno de los valores no sea cero para asegurar cálculo de fitness
    if (progressMade <= 0.0f && damageDealt <= 0.0f && timeAlive <= 0.0f) {
        std::cout << "ADVERTENCIA: Todos los valores de fitness son 0 o negativos!" << std::endl;
    }
    
    // Ejemplo de cálculo de fitness: priorizar el progreso hacia el puente
    float fitness = progressMade * 10.0f + (damageDealt / 100.0f) + (timeAlive / 1000.0f);
    
    // Asegurar que el fitness nunca sea negativo
    fitness = std::max(0.0f, fitness);
    
    // Almacenar el nuevo fitness
    genome.fitness = fitness;
    
    std::cout << "Fitness actualizado: " << fitness << std::endl;
}

void GeneticAlgorithm::evolve() {
    // Incrementar el contador de generaciones
    currentGeneration++;
    mutationsOccurred = 0;
    
    // 1. Calcular estadísticas antes de evolucionar
    updateStatistics();
    std::cout << "DEBUG - Antes de evolución: Fitness Prom=" << averageFitness 
              << ", Mejor=" << bestFitness << ", Peor=" << worstFitness << std::endl;
    
    // 2. Ordenar la población por fitness (de mayor a menor)
    std::sort(population.begin(), population.end(), 
              [](const Genome& a, const Genome& b) { return a.fitness > b.fitness; });
    
    // 3. Crear nueva población
    std::vector<Genome> newPopulation;
    
    // 3.1 Mantener a los "elite" individuos
    for (int i = 0; i < eliteCount && i < static_cast<int>(population.size()); i++) {
        newPopulation.push_back(population[i]);
    }
    
    // 3.2 Selección, cruce y mutación
    while (newPopulation.size() < populationSize) {
        // Seleccionar padres
        std::vector<Genome> parents = selectParents();
        
        // Crear hijo mediante cruce
        Genome child = crossover(parents[0], parents[1]);
        
        // Mutación
        mutate(child);
        
        // Añadir a la nueva población
        newPopulation.push_back(child);
    }
    
    // 3.3 Reemplazar población antigua con la nueva
    population = newPopulation;
    
    // 4. Actualizar estadísticas después de evolucionar
    updateStatistics();
    std::cout << "DEBUG - Después de evolución: Fitness Prom=" << averageFitness 
              << ", Mejor=" << bestFitness << ", Peor=" << worstFitness << std::endl;
    
    // Imprimir información
    std::cout << "Evolución completada. Generación " << currentGeneration 
              << ". Mutaciones: " << mutationsOccurred 
              << ". Mejor fitness: " << bestFitness << std::endl;
}

std::vector<Genome> GeneticAlgorithm::selectParents() {
    std::vector<Genome> parents;
    
    // Implementación de selección por torneo
    std::uniform_int_distribution<int> dist(0, population.size() - 1);
    
    // Seleccionar dos padres
    for (int i = 0; i < 2; i++) {
        // Torneo de 3 individuos
        int idx1 = dist(rng);
        int idx2 = dist(rng);
        int idx3 = dist(rng);
        
        // Seleccionar el mejor
        float fitness1 = population[idx1].fitness;
        float fitness2 = population[idx2].fitness;
        float fitness3 = population[idx3].fitness;
        
        if (fitness1 >= fitness2 && fitness1 >= fitness3) {
            parents.push_back(population[idx1]);
        } else if (fitness2 >= fitness1 && fitness2 >= fitness3) {
            parents.push_back(population[idx2]);
        } else {
            parents.push_back(population[idx3]);
        }
    }
    
    return parents;
}

Genome GeneticAlgorithm::crossover(const Genome& parent1, const Genome& parent2) {
    Genome child;
    
    // Distribución uniforme para el crossover
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Decidir si hay cruce
    if (dist(rng) < crossoverRate) {
        // Cruce de punto único: para cada atributo, 50% de probabilidad de cada padre
        child.enemyType = (dist(rng) < 0.5f) ? parent1.enemyType : parent2.enemyType;
        
        // Cruce de atributos numéricos: interpolación
        float alpha = dist(rng);  // Factor de mezcla
        
        child.health = parent1.health * alpha + parent2.health * (1.0f - alpha);
        child.speed = parent1.speed * alpha + parent2.speed * (1.0f - alpha);
        child.arrowResistance = parent1.arrowResistance * alpha + parent2.arrowResistance * (1.0f - alpha);
        child.magicResistance = parent1.magicResistance * alpha + parent2.magicResistance * (1.0f - alpha);
        child.artilleryResistance = parent1.artilleryResistance * alpha + parent2.artilleryResistance * (1.0f - alpha);
    } else {
        // Sin cruce: copiar uno de los padres directamente
        child = dist(rng) < 0.5f ? parent1 : parent2;
    }
    
    return child;
}

void GeneticAlgorithm::mutate(Genome& genome) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool mutated = false;
    
    // Mutación del tipo de enemigo (con menor probabilidad)
    if (dist(rng) < mutationRate * 0.3f) {
        std::uniform_int_distribution<int> typeDist(0, 3);
        genome.enemyType = typeDist(rng);
        mutated = true;
    }
    
    // Mutación de atributos
    if (dist(rng) < mutationRate) {
        genome.health += dist(rng) * 30.0f - 15.0f;  // ±15
        genome.health = std::max(limits.minHealth, std::min(limits.maxHealth, genome.health));
        mutated = true;
    }
    
    if (dist(rng) < mutationRate) {
        genome.speed += dist(rng) * 20.0f - 10.0f;  // ±10
        genome.speed = std::max(limits.minSpeed, std::min(limits.maxSpeed, genome.speed));
        mutated = true;
    }
    
    if (dist(rng) < mutationRate) {
        genome.arrowResistance += dist(rng) * 0.4f - 0.2f;  // ±0.2
        genome.arrowResistance = std::max(limits.minResistance, std::min(limits.maxResistance, genome.arrowResistance));
        mutated = true;
    }
    
    if (dist(rng) < mutationRate) {
        genome.magicResistance += dist(rng) * 0.4f - 0.2f;  // ±0.2
        genome.magicResistance = std::max(limits.minResistance, std::min(limits.maxResistance, genome.magicResistance));
        mutated = true;
    }
    
    if (dist(rng) < mutationRate) {
        // Las harpías deben mantener su inmunidad a la artillería
        if (genome.enemyType != 2) {  // No es harpía
            genome.artilleryResistance += dist(rng) * 0.4f - 0.2f;  // ±0.2
            genome.artilleryResistance = std::max(limits.minResistance, std::min(limits.maxResistance, genome.artilleryResistance));
            mutated = true;
        } else {
            // Las harpías siempre son inmunes a la artillería
            genome.artilleryResistance = 1.0f;
        }
    }
    
    if (mutated) {
        mutationsOccurred++;
    }
}

void GeneticAlgorithm::updateStatistics() {
    if (population.empty()) {
        averageFitness = 0.0f;
        bestFitness = 0.0f;
        worstFitness = 0.0f;
        std::cout << "ADVERTENCIA: Población vacía al calcular estadísticas!" << std::endl;
        return;
    }
    
    // Imprimir algunos fitness para depuración
    for (size_t i = 0; i < population.size(); i++) {
        std::cout << "DEBUG - Genoma " << i << " con fitness: " << population[i].fitness << std::endl;
    }
    
    // Calcular fitness promedio
    float totalFitness = 0.0f;
    for (const auto& genome : population) {
        totalFitness += genome.fitness;
    }
    
    averageFitness = totalFitness / population.size();
    
    // Encontrar mejor y peor fitness
    bestFitness = 0.0f;
    worstFitness = std::numeric_limits<float>::max();
    
    for (const auto& genome : population) {
        if (genome.fitness > bestFitness) {
            bestFitness = genome.fitness;
        }
        if (genome.fitness < worstFitness && genome.fitness > 0.0f) {
            worstFitness = genome.fitness;
        }
    }
    
    // Si todos tienen fitness 0, hacer que worstFitness sea 0
    if (worstFitness == std::numeric_limits<float>::max()) {
        worstFitness = 0.0f;
    }
    
    std::cout << "ESTADÍSTICAS ACTUALIZADAS: Promedio=" << averageFitness 
              << ", Mejor=" << bestFitness 
              << ", Peor=" << worstFitness << std::endl;
}