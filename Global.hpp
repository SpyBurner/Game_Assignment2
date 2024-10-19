#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <SDL2/SDL.h>
#include <vector>

extern SDL_Renderer* RENDERER;
extern std::vector<SDL_Texture *> TEXTURES;

//SETTINGS
const int FPS = 60;
const int WIDTH = 1280, HEIGHT = 720;
const bool FULLSCREEN = false;

const float HIGH_KICK_FORCE = 17.0f;
const float LOW_KICK_FORCE = 12.0f;

const bool Player2Mode = false;

#define EPS 0.0001

#endif // GLOBALS_HPP