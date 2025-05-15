# Compilador
CXX = g++

# Banderas del compilador
CXXFLAGS = -std=c++17 -Wall -Wextra

# Banderas para SDL2
SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

# Directorio de código fuente
SRC_DIR = src

# Encontrar todos los archivos .cpp
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Crear nombres de archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Nombre del ejecutable
TARGET = genetic_kingdom

# Regla principal
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LIBS)

# Compilar archivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run