#include <iostream>

#include "Game.hpp"
#include <SDL2/SDL.h>


const int WIDTH = 800, HEIGHT = 600;

Game *game = nullptr;

int main(int argc, char *argv[]) {
    game = new Game();

    game->init("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, false);

    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();
    }

    game->clean();

    return 0;
}