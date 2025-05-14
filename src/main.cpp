#include "Game.h"

int main(int, char**) {
    Game game;
    
    if (game.initialize()) {
        game.run();
    }
    
    return 0;
}