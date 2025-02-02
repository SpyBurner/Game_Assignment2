#ifndef GAME_HPP
#define GAME_HPP

#include<SDL2/SDL.h>
class Game{

public:    
    Game();
    ~Game();

    enum State{
        MENU,
        GAME,
        GAMEOVER
    };

    State state = MENU;

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void objectInit();
    void handleEvents();
    void handleSceneChange();
    void update();
    void render();
    void clean();

    bool running();
    bool reseting();

    static SDL_Event event;

    int scoreTeam1 = 0;
    int scoreTeam2 = 0;

private: 
    bool isRunning;
    bool reset = false;
    SDL_Window *window;
    SDL_Renderer *renderer;    
};

#endif // GAME_HPP