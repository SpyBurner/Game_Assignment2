#include "Game.hpp"
#include "Global.hpp"
#include "CustomClasses.hpp"
#include <iostream>

Game::Game() {
    isRunning = false;
}

Game::~Game() {
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems Initialised..." << std::endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            std::cout << "Window created..." << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "Renderer created..." << std::endl;
        }

        RENDERER = renderer;

        isRunning = true;
    } else {
        isRunning = false;
    }

    objectInit();
}

void dummy(){
    std::cout << "Dummy" << std::endl;
}

void Game::objectInit() {
    // Add your object initialisation here

    std::cout << "Object Initialisation..." << std::endl;

    //Test scene
    Scene *main = new Scene("Main");
    GameObject *player = new GameObject("Player");
    player->transform.position = Vector2(100, 100);
    player->AddComponent(new SpriteRenderer(player, renderer, Vector2(35, 44)));

    SpriteRenderer* spriteRenderer = player->GetComponent<SpriteRenderer>();
     spriteRenderer->spriteSheet = LoadSpriteSheet("Assets/default.png");

    // AnimationClip playerIdle = AnimationClip("Idle", "Assets/kirby_fall.png", Vector2(35, 44), 7, true, 2.0, 0, 14);
    // playerIdle.onComplete->addHandler(dummy);

    // player->AddComponent(new Animator(player, {playerIdle}));

    // player->GetComponent<Animator>()->Play("Idle");

    main->AddGameObject(player);

    SceneManager::GetInstance()->AddScene(main);
    SceneManager::GetInstance()->LoadScene("Main");
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::update() {
    // Game logic updates go here
    SceneManager::GetInstance()->Update();
}

void Game::render() {
    SDL_RenderClear(renderer);
    // Add your rendering stuff here
    SceneManager::GetInstance()->Draw();
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    delete SceneManager::GetInstance();
    
    for (auto &texture: TEXTURES) {
        SDL_DestroyTexture(texture);
    }
    TEXTURES.clear();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game cleaned..." << std::endl;
}

bool Game::running() {
    return isRunning;
}