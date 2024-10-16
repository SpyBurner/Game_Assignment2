#ifndef GAME_HPP
#define GAME_HPP

#include<SDL2/SDL.h>
class Game{

public:    
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void objectInit();
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running();

private: 
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;    
};

#endif // GAME_HPP