#include "../include/Game.hpp"

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.initialize()) {
        return 1;
    }

    game.run();
    game.cleanup();
    
    return 0;
}