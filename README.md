# Genetic Kingdom

![Banner del Juego](images/banner.png)

Un juego tower defense donde los enemigos evolucionan mediante algoritmos genéticos para superar tus defensas. Desarrollado con C++ y SDL2.

## Descripción General

Genetic Kingdom es un juego tower defense ambientado en la época medieval donde defiendes tu castillo contra oleadas de enemigos que evolucionan. A medida que eliminas enemigos, estos se adaptan a tu estrategia mediante algoritmos genéticos, volviéndose progresivamente más desafiantes. El juego implementa el algoritmo de pathfinding A* para el movimiento de enemigos y algoritmos genéticos para la evolución de los mismos.

## ✨ Características

- Tres tipos únicos de torres (Arqueros, Magos, Artillería) con diferentes habilidades
- Cuatro clases de enemigos (Ogros, Elfos Oscuros, Harpías, Mercenarios) con resistencias variadas
- Los enemigos evolucionan después de cada oleada usando algoritmos genéticos
- Pathfinding A* para movimiento inteligente de enemigos
- Sistema de mejora de torres con ataques especiales
- Sistema de retroalimentación de combate en tiempo real
- Gestión dinámica de recursos

## 🚀 Instalación

### Requisitos Previos

- Compilador compatible con C++17 (se recomienda g++ 11.2.0 o más reciente)
- Bibliotecas SDL2, SDL2_image y SDL2_ttf

### Instalación en Linux

1. Instalar dependencias:
   ```bash
   sudo apt update
   sudo apt install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
   ```

2. Clonar el repositorio:
   ```bash
   git clone https://github.com/ned8120/GeneticKingdom
   cd genetic-kingdom
   ```

3. Compilar el juego:
   ```bash
   make
   ```

4. Ejecutar el juego:
   ```bash
   ./genetic_kingdom
   ```

## 🎮 Cómo Jugar

1. **Inicia el juego** ejecutando el programa.
2. **Construye torres** seleccionando un tipo de torre desde el menú superior y haciendo clic en un espacio vacío del mapa.
3. **Defiende tu castillo** de las oleadas de enemigos que intentan cruzar el puente.
4. **Mejora tus torres** seleccionando una torre existente y haciendo clic en el botón de mejora.
5. **Adapta tu estrategia**: los enemigos evolucionarán según las torres que usaste previamente.
