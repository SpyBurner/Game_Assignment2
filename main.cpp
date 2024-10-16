#include "Game.hpp"
#include "Global.hpp"
#include <SDL2/SDL.h>
#include <iostream>


Game *game = nullptr;

int main(int argc, char *argv[]) {
    game = new Game();

    game->init("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, false);

    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();
        
        SDL_Delay(1000 / FPS);
    }

    game->clean();

    return 0;
}