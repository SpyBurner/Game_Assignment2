#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <SDL2/SDL.h>
#include <vector>

extern SDL_Renderer* RENDERER;
extern std::vector<SDL_Texture *> TEXTURES;

//SETTINGS
const int FPS = 60;
const int WIDTH = 800, HEIGHT = 600;

#endif // GLOBALS_HPP